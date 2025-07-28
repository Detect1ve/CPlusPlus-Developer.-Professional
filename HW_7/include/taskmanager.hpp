#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <chrono>
#include <iostream>
#if __GNUC__ < 14
#include <vector>
#endif

namespace bulk
{
    class taskmanager
    {
    public:
        taskmanager(
            const int        max_task_count,
            std::string_view name)
            :
            max_static_task_count(max_task_count),
            task_manager_name(name) { };

        taskmanager(const taskmanager&) = delete;
        taskmanager& operator=(const taskmanager&) = delete;
        taskmanager(taskmanager&&) = delete;
        taskmanager& operator=(taskmanager&&) = delete;

        [[nodiscard]] int run(std::istream& input);
        [[nodiscard]] int run();

        ~taskmanager() noexcept;

    private:
        int add_task(std::string_view task);
        [[nodiscard]] bool is_dynamic_block_active() const noexcept;
        int process_tasks(std::vector<std::string>& block_task) noexcept;

        int dynamic_block_nesting_level = 0;
        int task_count = 0;
        int max_static_task_count;
        std::chrono::system_clock::time_point dynamic_block_timestamp;
        std::chrono::system_clock::time_point static_block_timestamp;
        std::string task_manager_name;
        std::vector<std::string> dynamic_block_task;
        std::vector<std::string> static_block_task;
    };
} // namespace bulk

#endif /* TASKMANAGER_HPP */
