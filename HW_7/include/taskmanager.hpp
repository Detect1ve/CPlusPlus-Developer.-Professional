#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <atomic>
#include <chrono>
#include <iostream>
#if __GNUC__ < 14
#include <vector>
#endif

namespace bulk
{
    class taskmanager
    {
        int add_task(std::string_view task);
        [[nodiscard]] bool is_dynamic_block_active() const noexcept
#ifndef _MSC_VER
            __attribute__((pure))
#endif
            ;
        int process_tasks(std::vector<std::string>& block_task);

        int dynamic_block_nesting_level = 0;
        int task_count = 0;
        int max_static_task_count;
        std::atomic<bool> stop_flag;
        std::chrono::system_clock::time_point dynamic_block_timestamp;
        std::chrono::system_clock::time_point static_block_timestamp;
        std::string task_manager_name;
        std::vector<std::string> dynamic_block_task;
        std::vector<std::string> static_block_task;
    public:
        taskmanager(
            const int        max_task_count,
            std::string_view name)
            :
            max_static_task_count(max_task_count),
            stop_flag(false),
            task_manager_name(name) {}

        taskmanager(const taskmanager&) = delete;
        taskmanager& operator=(const taskmanager&) = delete;
        taskmanager(taskmanager&&) = delete;
        taskmanager& operator=(taskmanager&&) = delete;

        [[nodiscard]] int run(std::istream& input);
        [[nodiscard]] int run();
        void setup_signal_handling();
        void stop() noexcept;

        ~taskmanager() noexcept;
    };
} // namespace bulk

#endif // TASKMANAGER_HPP
