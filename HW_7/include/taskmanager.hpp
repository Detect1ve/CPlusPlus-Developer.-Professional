#pragma once

#include <chrono>
#include <iostream>
#if __GNUC__ < 15
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

        [[nodiscard]] auto run(std::istream& input = std::cin) -> int;

        ~taskmanager() noexcept;

    private:
        auto add_task(std::string_view task) -> int;
        [[nodiscard]] auto is_dynamic_block_active() const noexcept -> bool;
        auto process_tasks(std::vector<std::string>& block_task) noexcept -> int;

        int dynamic_block_nesting_level = 0;
        int task_count = 0;
        int max_static_task_count;
        std::chrono::system_clock::time_point dynamic_block_timestamp =
            std::chrono::system_clock::time_point{};
        std::chrono::system_clock::time_point static_block_timestamp =
            std::chrono::system_clock::time_point{};
        std::string task_manager_name;
        std::vector<std::string> dynamic_block_task;
        std::vector<std::string> static_block_task;
    };
} // namespace bulk
