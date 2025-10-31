#if __cplusplus <= 201703L
#include <atomic>
#include <thread>
#endif
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <chrono>
#endif
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>

#include "async.h"

namespace {
const std::string& TASK_MANAGER_NAME() {
    static const std::string s_name = "bulk";

    return s_name;
}
} // namespace

namespace async {

struct OutputTask {
    OutputTask() = default;
    OutputTask(std::chrono::system_clock::time_point timestamp, std::string&& context_id,
        std::vector<std::string>&& commands) : timestamp_(timestamp),
        context_id_(std::move(context_id)), commands_(std::move(commands)) {}

    [[nodiscard]] const std::chrono::system_clock::time_point& get_timestamp() const {
        return timestamp_;
    }

    [[nodiscard]] const std::string& get_context_id() const {
        return context_id_;
    }

    [[nodiscard]] const std::vector<std::string>& get_commands() const {
        return commands_;
    }
private:
    std::chrono::system_clock::time_point timestamp_;
    std::string context_id_;
    std::vector<std::string> commands_;
};

class taskmanager {
    int dynamic_block_nesting_level = 0;
    std::chrono::system_clock::time_point dynamic_block_timestamp;
    std::chrono::system_clock::time_point static_block_timestamp;
    std::size_t max_static_task_count;
    std::string task_manager_id;
    std::vector<std::string> dynamic_block_task;
    std::vector<std::string> static_block_task;
public:
    taskmanager(const std::size_t max_task_count, std::string_view manager_id) :
        max_static_task_count(max_task_count), task_manager_id(manager_id) {}

    [[nodiscard]] bool is_dynamic_block_active() const noexcept {
        return dynamic_block_nesting_level > 0;
    }

    void add_task(std::string_view task) {
        static constexpr std::string_view CLOSE_BRACKET = "}";
        static constexpr std::string_view OPEN_BRACKET = "{";

        if (task == OPEN_BRACKET) {
            if (!is_dynamic_block_active()) {
                dynamic_block_timestamp = std::chrono::system_clock::now();
                process_tasks(static_block_task);
            }

            dynamic_block_nesting_level++;

            return;
        }

        if (task == CLOSE_BRACKET) {
            if (dynamic_block_nesting_level > 0) {
                dynamic_block_nesting_level--;
                if (!is_dynamic_block_active()) {
                    process_tasks(dynamic_block_task);
                }

                return;
            }
        }

        if (is_dynamic_block_active()) {
            dynamic_block_task.emplace_back(task);
        } else {
            if (static_block_task.empty()) {
                static_block_timestamp = std::chrono::system_clock::now();
            }

            static_block_task.emplace_back(task);
            if (static_block_task.size() >= max_static_task_count) {
                process_tasks(static_block_task);
            }
        }
    }

    void process_tasks(std::vector<std::string>& block_task) {
        if (block_task.empty()) [[unlikely]] {
            return;
        }

        const bool is_static = (&block_task == &static_block_task);
        auto& timestamp = (is_static ? static_block_timestamp : dynamic_block_timestamp);
        const OutputTask task(timestamp, std::string(task_manager_id),
            std::move(block_task));

        {
            const std::scoped_lock<std::mutex> lock(queue_mutex());
            log_queue().push(task);
            file_queue().push(task);
        }

        queue_cv().notify_all();

        block_task.clear();
        timestamp = std::chrono::system_clock::time_point{};
    }

    void finish() {
        if (!is_dynamic_block_active() && !static_block_task.empty()) {
            process_tasks(static_block_task);
        }
    }

    static std::atomic<bool> should_terminate;
    static std::atomic<bool> threads_initialized;

    static std::condition_variable& queue_cv() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* condition = new std::condition_variable();

        return *condition;
    }

    static std::mutex& queue_mutex() {
        static std::mutex mutex;

        return mutex;
    }

    static std::queue<OutputTask>& file_queue() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* queue = new std::queue<OutputTask>;

        return *queue;
    }

    static std::queue<OutputTask>& log_queue() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* queue = new std::queue<OutputTask>;

        return *queue;
    }

    static std::thread& file_thread1() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* thread = new std::thread;

        return *thread;
    }

    static std::thread& file_thread2() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* thread = new std::thread;

        return *thread;
    }

    static std::thread& log_thread() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        static auto* thread = new std::thread;

        return *thread;
    }
};

std::atomic<bool> taskmanager::should_terminate{false};
std::atomic<bool> taskmanager::threads_initialized{false};

namespace {
std::mutex& contexts_mutex() {
    static std::mutex mutex;

    return mutex;
}

std::map<void*, std::unique_ptr<taskmanager>>& contexts() {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    static auto* contexts_map = new std::map<void*, std::unique_ptr<taskmanager>>;

    return *contexts_map;
}

void log_worker() {
    while (true) {
        OutputTask task;
        {
            std::unique_lock<std::mutex> lock(taskmanager::queue_mutex());
            taskmanager::queue_cv().wait(lock, [] {
                return !taskmanager::log_queue().empty() || taskmanager::should_terminate;
            });

            if (taskmanager::should_terminate && taskmanager::log_queue().empty()) {
                break;
            }

            if (!taskmanager::log_queue().empty()) {
                task = taskmanager::log_queue().front();
                taskmanager::log_queue().pop();
            }
        }

        if (!task.get_commands().empty()) {
            std::cout << TASK_MANAGER_NAME() << ": ";

            const std::string_view delimiter = ", ";
            for (std::size_t i = 0; i < task.get_commands().size(); ++i) {
                std::cout << task.get_commands()[i];
                if (i < task.get_commands().size() - 1) {
                    std::cout << delimiter;
                }
            }

            std::cout << '\n';
        }
    }
}

void file_worker(const int thread_id) {
    static std::atomic<int> file_counter{0};
    while (true) {
        OutputTask task;
        bool has_task = false;
        {
            std::unique_lock<std::mutex> lock(taskmanager::queue_mutex());
            taskmanager::queue_cv().wait(lock, [] {
                return   !taskmanager::file_queue().empty()
                      || taskmanager::should_terminate;
            });

            if (taskmanager::should_terminate && taskmanager::file_queue().empty()) {
                break;
            }

            if (!taskmanager::file_queue().empty()) {
                task = taskmanager::file_queue().front();
                taskmanager::file_queue().pop();
                has_task = true;
            }
        }

        if (has_task && !task.get_commands().empty()) {
            auto timestamp_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                task.get_timestamp().time_since_epoch()).count();

            const std::string filename = TASK_MANAGER_NAME()
                + std::to_string(timestamp_seconds) + "_" + task.get_context_id() + "_"
                + std::to_string(thread_id) + "_" + std::to_string(file_counter++)
                + ".log";

            std::ofstream file(filename);
            if (file.is_open()) {
                file << TASK_MANAGER_NAME() << ": ";

                const std::string delimiter = ", ";
                for (std::size_t i = 0; i < task.get_commands().size(); ++i) {
                    file << task.get_commands()[i];
                    if (i < task.get_commands().size() - 1) {
                        file << delimiter;
                    }
                }

                file << '\n';
                file.close();
            }
        }
    }
}

void init_threads() {
    if (!taskmanager::threads_initialized.exchange(true)) {
        taskmanager::log_thread() = std::thread(log_worker);
        taskmanager::file_thread1() = std::thread(file_worker, 1);
        taskmanager::file_thread2() = std::thread(file_worker, 2);
    }
}

void stop_threads() {
    if (taskmanager::threads_initialized.exchange(false)) {
        taskmanager::should_terminate = true;
        taskmanager::queue_cv().notify_all();

        if (taskmanager::log_thread().joinable()) {
            taskmanager::log_thread().join();
        }

        if (taskmanager::file_thread1().joinable()) {
            taskmanager::file_thread1().join();
        }

        if (taskmanager::file_thread2().joinable()) {
            taskmanager::file_thread2().join();
        }
    }
}
} // namespace

handle_t connect(std::size_t bulk) {
    init_threads();

    static std::atomic<int> next_id{0};
    const int unique_id = next_id++;
    const std::string context_id = std::to_string(unique_id);

    auto context = std::make_unique<taskmanager>(bulk, context_id);
    auto *handle = static_cast<handle_t>(context.get());

    const std::scoped_lock<std::mutex> lock(contexts_mutex());
    contexts()[handle] = std::move(context);

    return handle;
}

void receive(handle_t handle, const char *data, std::size_t size) {
    if (handle == nullptr) {
        return;
    }

    taskmanager* context = nullptr;
    {
        const std::scoped_lock<std::mutex> lock(contexts_mutex());
        auto context_iterator = contexts().find(handle);
        if (context_iterator != contexts().end()) {
            context = context_iterator->second.get();
        }
    }

    if (context == nullptr) {
        return;
    }

    const std::string input(data, size);
    std::istringstream stream(input);
    std::string command;

    while (std::getline(stream, command)) {
        if (!command.empty()) {
            context->add_task(command);
        }
    }
}

void disconnect(handle_t handle) {
    if (handle == nullptr) {
        return;
    }

    std::unique_ptr<taskmanager> context;
    {
        const std::scoped_lock<std::mutex> lock(contexts_mutex());
        auto context_iterator = contexts().find(handle);
        if (context_iterator != contexts().end()) {
            context_iterator->second->finish();
            context = std::move(context_iterator->second);
            contexts().erase(context_iterator);
        }
    }

    if (contexts().empty()) {
        stop_threads();
    }
}
// NOLINTNEXTLINE(google-readability-namespace-comments,llvm-namespace-comment)
}
