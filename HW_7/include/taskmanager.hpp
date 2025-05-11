#pragma once

#include <iostream>
#include <vector>

namespace bulk
{
    class taskmanager
    {
    public:
        taskmanager(
            const int max_task_count,
            const std::string& name)
            :
            max_static_task_count(max_task_count),
            task_manager_name(name) { };

        auto run(std::istream& input = std::cin) -> int;

        ~taskmanager();

    private:
        auto add_task(std::string_view task) -> int;
        [[nodiscard]] auto is_dynamic_block_active() const -> bool;
        auto process_tasks(std::vector<std::string>& block_task) -> int;

        int dynamic_block_nesting_level = 0;
        int task_count = 0;
        int max_static_task_count;
        std::string task_manager_name;
        std::vector<std::string> dynamic_block_task;
        std::vector<std::string> static_block_task;
        time_t dynamic_block_timestamp = 0;
        time_t static_block_timestamp = 0;
    };
} // namespace bulk
