#include "async.h"

namespace async {

class taskmanager {
    int dynamic_block_nesting_level = 0;
    int max_static_task_count;
    std::chrono::system_clock::time_point dynamic_block_timestamp;
    std::chrono::system_clock::time_point static_block_timestamp;
    std::string task_manager_name;
    std::vector<std::string> dynamic_block_task;
    std::vector<std::string> static_block_task;
public:
    taskmanager(const int max_task_count, std::string_view name):
        max_static_task_count(max_task_count), task_manager_name(name) {};

    [[nodiscard]] bool is_dynamic_block_active() const noexcept {
        return dynamic_block_nesting_level > 0;
    }

    int add_task(std::string_view task) {
        static constexpr std::string_view CLOSE_BRACKET = "}";
        static constexpr std::string_view OPEN_BRACKET = "{";
        int ret = 0;

        if (task == OPEN_BRACKET) {
            if (!is_dynamic_block_active()) {
                dynamic_block_timestamp = std::chrono::system_clock::now();
                ret = process_tasks(static_block_task);
            }

            dynamic_block_nesting_level++;

            return ret;
        }

        if (task == CLOSE_BRACKET) {
            dynamic_block_nesting_level--;
            if (!is_dynamic_block_active()) {
                ret = process_tasks(dynamic_block_task);
            }
            // Special case, when user try to enter "}" string as command
            if (dynamic_block_nesting_level < 0) {
                dynamic_block_nesting_level = 0;
            } else {
                return ret;
            }
        }

        if (is_dynamic_block_active()) {
            dynamic_block_task.emplace_back(task);
        } else {
            if (static_block_timestamp == std::chrono::system_clock::time_point{}) {
                static_block_timestamp = std::chrono::system_clock::now();
            }

            static_block_task.emplace_back(task);
            if (++task_count % max_static_task_count == 0) {
                ret = process_tasks(static_block_task);
            }
        }

        return ret;
    }

    int process_tasks(std::vector<std::string>& block_task) {
        int ret = 0;

        if (block_task.empty()) [[unlikely]] {
            return ret;
        }

        std::chrono::system_clock::time_point *timestamp =
            (task_count != 0 ? &static_block_timestamp : &dynamic_block_timestamp);

        auto timestamp_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            timestamp->time_since_epoch()).count();

        const std::filesystem::path log_path =
            std::filesystem::path(task_manager_name).filename().string() +
            std::to_string(timestamp_seconds) + ".log";

        std::ofstream file(log_path);

        if (!file.is_open()) {
            std::cerr << "Failed to open file\n";
            ret = -1;

            return ret;
        }

        bulk::io::composite_stream output(std::cout, file);

        output << std::filesystem::path(task_manager_name).filename().string() << ": ";

        const std::string_view delimiter = ", ";
#ifdef __cpp_lib_ranges_to_container
        const std::string result = block_task | std::views::join_with(delimiter)
            | std::ranges::to<std::string>();
#else
        std::string result;

        for (std::size_t i = 0; i < block_task.size(); ++i) {
            result += block_task[i];
            if (i + 1 < block_task.size()) {
                result += delimiter;
            }
        }
#endif
        output << result;
        output << '\n';

        block_task.clear();

        *timestamp = std::chrono::system_clock::time_point{};
        task_count = 0;

        return ret;
    }
}

handle_t connect(std::size_t bulk) {
    return nullptr;
}

void receive(handle_t handle, const char *data, std::size_t size) {
}

void disconnect(handle_t handle) {
}

}
