#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <atomic>
#include <chrono>
#if __GNUC__ < 14
#include <vector>
#endif

#include <attribute_wrapper.hpp>

namespace bulk
{
    class taskmanager
    {
    public:
        taskmanager(
            const int              max_task_count,
            const std::string_view name)
            :
            max_static_task_count_(max_task_count),
            stop_flag_(false),
            task_manager_name_(name) {}
        ~taskmanager() noexcept;
        taskmanager(const taskmanager&) = delete;
        taskmanager& operator=(const taskmanager&) = delete;
        taskmanager(taskmanager&&) = delete;
        taskmanager& operator=(taskmanager&&) = delete;

        [[nodiscard]] int run(std::istream& input);
        [[nodiscard]] int run();
        void setup_signal_handling();
        void stop() noexcept;

    private:
        int add_task(std::string_view task);
        ATTRIBUTE_PURE [[nodiscard]] bool is_dynamic_block_active() const noexcept;
        int process_tasks(std::vector<std::string>& block_task);

        int dynamic_block_nesting_level_ = 0;
        int task_count_ = 0;
        int max_static_task_count_;
        std::atomic<bool> stop_flag_;
        std::chrono::system_clock::time_point dynamic_block_timestamp_;
        std::chrono::system_clock::time_point static_block_timestamp_;
        std::string task_manager_name_;
        std::vector<std::string> dynamic_block_task_;
        std::vector<std::string> static_block_task_;
    };
} // namespace bulk

#endif // TASKMANAGER_HPP
