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
    std::chrono::system_clock::time_point timestamp;
    std::string context_id;
    std::vector<std::string> commands;
};

class taskmanager {
public:
    explicit taskmanager(const std::size_t max_task_count):
        max_static_task_count(max_task_count) { };

    void add_task(std::string_view task) {
        if (static_block_task.empty()) {
            static_block_timestamp = std::chrono::system_clock::now();
        }

        static_block_task.emplace_back(task);
        if (static_block_task.size() >= max_static_task_count) {
            process_tasks(static_block_task, static_block_timestamp);
        }
    }

    static void process_tasks(std::vector<std::string>& block_task,
        const std::chrono::system_clock::time_point& timestamp) {
        process_tasks(block_task, timestamp, "0");
    }

    static void process_tasks(std::vector<std::string>& block_task,
        const std::chrono::system_clock::time_point& timestamp,
        const std::string& context_id) {
        if (block_task.empty()) [[unlikely]] {
            return;
        }

        OutputTask task;
        task.commands = block_task;
        task.timestamp = timestamp;
        task.context_id = context_id;

        {
            const std::lock_guard<std::mutex> lock(queue_mutex());
            log_queue().push(task);
            file_queue().push(task);
        }

        queue_cv().notify_all();

        block_task.clear();
    }

    void finish() {
        if (!static_block_task.empty()) {
            process_tasks(static_block_task, static_block_timestamp);
        }
    }

    static std::atomic<bool> should_terminate;
    static std::atomic<bool> threads_initialized;

    static std::condition_variable& queue_cv() {
        static std::condition_variable condition;

        return condition;
    }

    static std::mutex& queue_mutex() {
        static std::mutex mutex;

        return mutex;
    }

    static std::queue<OutputTask>& file_queue() {
        static std::queue<OutputTask> queue;

        return queue;
    }

    static std::queue<OutputTask>& log_queue() {
        static std::queue<OutputTask> queue;

        return queue;
    }

    static std::thread& file_thread1() {
        static std::thread thread;

        return thread;
    }

    static std::thread& file_thread2() {
        static std::thread thread;

        return thread;
    }

    static std::thread& log_thread() {
        static std::thread thread;

        return thread;
    }

private:
    std::size_t max_static_task_count;
    std::chrono::system_clock::time_point static_block_timestamp;
    std::vector<std::string> static_block_task;
};

std::atomic<bool> taskmanager::should_terminate{false};
std::atomic<bool> taskmanager::threads_initialized{false};

struct ConnectionContext {
    int dynamic_block_nesting_level = 0;
    std::vector<std::string> dynamic_block_task;
    std::chrono::system_clock::time_point dynamic_block_timestamp;
    std::string id;
};

namespace {
    std::mutex& contexts_mutex() {
        static std::mutex mutex;

        return mutex;
    }

    std::map<void*, std::unique_ptr<ConnectionContext>>& contexts() {
        static std::map<void*, std::unique_ptr<ConnectionContext>> contexts_map;

        return contexts_map;
    }

    std::unique_ptr<taskmanager>& shared_task_manager() {
        static std::unique_ptr<taskmanager> manager;

        return manager;
    }

    void process_command(ConnectionContext* context, const std::string& command) {
        if (command == "{") {
            if (context->dynamic_block_nesting_level == 0) {
                if (shared_task_manager()) {
                    shared_task_manager()->finish();
                }

                context->dynamic_block_timestamp = std::chrono::system_clock::now();
            }

            context->dynamic_block_nesting_level++;
        } else if (command == "}") {
            context->dynamic_block_nesting_level--;
            if (context->dynamic_block_nesting_level == 0) {
                if (shared_task_manager()) {
                    taskmanager::process_tasks(context->dynamic_block_task,
                        context->dynamic_block_timestamp, context->id);
                }
            }
        } else if (!command.empty()) {
            if (context->dynamic_block_nesting_level > 0) {
                context->dynamic_block_task.emplace_back(command);
            } else {
                if (shared_task_manager()) {
                    shared_task_manager()->add_task(command);
                }
            }
        }
    }

    void log_worker() {
        while (true) {
            OutputTask task;
            {
                std::unique_lock<std::mutex> lock(taskmanager::queue_mutex());
                taskmanager::queue_cv().wait(lock, [] {
                    return   !taskmanager::log_queue().empty()
                          || taskmanager::should_terminate;
                });

                if (taskmanager::should_terminate && taskmanager::log_queue().empty()) {
                    break;
                }

                if (!taskmanager::log_queue().empty()) {
                    task = taskmanager::log_queue().front();
                    taskmanager::log_queue().pop();
                }
            }

            if (!task.commands.empty()) {
                std::cout << TASK_MANAGER_NAME() << ": ";

                const std::string_view delimiter = ", ";
                for (std::size_t i = 0; i < task.commands.size(); ++i) {
                    std::cout << task.commands[i];
                    if (i < task.commands.size() - 1) {
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

            if (has_task && !task.commands.empty()) {
                auto timestamp_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                    task.timestamp.time_since_epoch()).count();

                const std::string filename = TASK_MANAGER_NAME() +
                    std::to_string(timestamp_seconds) + "_" + task.context_id + "_" +
                    std::to_string(thread_id) + "_" + std::to_string(file_counter++) +
                    ".log";

                std::ofstream file(filename);
                if (file.is_open()) {
                    file << TASK_MANAGER_NAME() << ": ";

                    const std::string delimiter = ", ";
                    for (std::size_t i = 0; i < task.commands.size(); ++i) {
                        file << task.commands[i];
                        if (i < task.commands.size() - 1) {
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
            taskmanager::should_terminate = false;

            taskmanager::log_thread() = std::thread(log_worker);
            taskmanager::file_thread1() = std::thread(file_worker, 1);
            taskmanager::file_thread2() = std::thread(file_worker, 2);
        }
    }

    void stop_threads() {
        if (taskmanager::threads_initialized.exchange(false)) {
            if(shared_task_manager()) {
                shared_task_manager()->finish();
            }

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

            if (shared_task_manager()) {
                shared_task_manager().reset();
            }
        }
    }
} // namespace

handle_t connect(std::size_t bulk) {
    const std::lock_guard<std::mutex> lock(contexts_mutex());
    if (!shared_task_manager()) {
        shared_task_manager() = std::make_unique<taskmanager>(bulk);
        init_threads();
    }

    static std::atomic<int> next_id{0};
    const int unique_id = next_id++;
    
    auto context = std::make_unique<ConnectionContext>();
    context->id = std::to_string(unique_id);
    auto *handle = static_cast<handle_t>(context.get());
    contexts()[handle] = std::move(context);

    return handle;
}

void receive(handle_t handle, const char *data, std::size_t size) {
    if (handle == nullptr) {
        return;
    }

    ConnectionContext* context = nullptr;
    {
        const std::lock_guard<std::mutex> lock(contexts_mutex());
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
        process_command(context, command);
    }
}

void disconnect(handle_t handle) {
    if (handle == nullptr) {
        return;
    }

    std::unique_ptr<ConnectionContext> context;
    {
        const std::lock_guard<std::mutex> lock(contexts_mutex());
        auto context_iterator = contexts().find(handle);
        if (context_iterator != contexts().end()) {
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
