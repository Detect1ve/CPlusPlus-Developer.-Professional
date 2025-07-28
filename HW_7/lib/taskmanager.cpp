#include <filesystem>
#include <fstream>

#include <taskmanager.hpp>

namespace bulk::io
{
    class composite_stream
    {
    public:
        composite_stream(
            std::ostream& stream1,
            std::ostream& stream2)
            :
            s1(stream1),
            s2(stream2) {}

        composite_stream(const composite_stream&) = delete;
        composite_stream& operator=(const composite_stream&) = delete;
        composite_stream(composite_stream&&) = delete;
        composite_stream& operator=(composite_stream&&) = delete;
        ~composite_stream() = default;


        template<typename T>
        composite_stream& operator<<(const T& value)
        {
            if constexpr (std::is_array_v<std::remove_reference_t<T>>)
            {
                s1 << static_cast<const char*>(value);
                s2 << static_cast<const char*>(value);
            }
            else
            {
                s1 << value;
                s2 << value;
            }

            return *this;
        }

        composite_stream& operator<<(std::ostream& (*manip)(std::ostream&)) 
        {
            s1 << manip;
            s2 << manip;

            return *this;
        }

    private:
        std::ostream& s1;
        std::ostream& s2;
    };
} // namespace bulk::io

namespace bulk
{
    [[nodiscard]] bool taskmanager::is_dynamic_block_active() const noexcept
    {
        return dynamic_block_nesting_level > 0;
    }

    int taskmanager::add_task(std::string_view task)
    {
        static constexpr std::string_view CLOSE_BRACKET = "}";
        static constexpr std::string_view OPEN_BRACKET = "{";
        int ret = 0;

        if (task == OPEN_BRACKET)
        {
            if (!taskmanager::is_dynamic_block_active())
            {
                dynamic_block_timestamp = std::chrono::system_clock::now();
                ret = process_tasks(static_block_task);
            }

            dynamic_block_nesting_level++;

            return ret;
        }

        if (task == CLOSE_BRACKET)
        {
            dynamic_block_nesting_level--;
            if (!taskmanager::is_dynamic_block_active())
            {
                ret = process_tasks(dynamic_block_task);
            }
            // Special case, when user try to enter "}" string as command
            if (dynamic_block_nesting_level < 0)
            {
                dynamic_block_nesting_level = 0;
            }
            else
            {
                return ret;
            }
        }

        if (taskmanager::is_dynamic_block_active())
        {
            dynamic_block_task.emplace_back(task);
        }
        else
        {
            if (static_block_timestamp == std::chrono::system_clock::time_point{})
            {
                static_block_timestamp = std::chrono::system_clock::now();
            }

            static_block_task.emplace_back(task);
            if (++task_count % max_static_task_count == 0)
            {
                ret = process_tasks(static_block_task);
            }
        }

        return ret;
    }

    int taskmanager::process_tasks(std::vector<std::string>& block_task) noexcept
    {
        int ret = 0;

        if (block_task.empty()) [[unlikely]]
        {
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

        if (!file.is_open())
        {
            std::cerr << "Failed to open file" << '\n';
            ret = -1;

            return ret;
        }

        bulk::io::composite_stream output(std::cout, file);

        output << std::filesystem::path(task_manager_name).filename().string() << ": ";

        const std::string_view delimiter = ", ";
#if defined(__cpp_lib_ranges_to_container)
        std::string result = block_task | std::views::join_with(delimiter)
            | std::ranges::to<std::string>();
#else
        std::string result;

        for (std::size_t i = 0; i < block_task.size(); ++i)
        {
            result += block_task[i];
            if (i + 1 < block_task.size())
            {
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

    int taskmanager::run()
    {
        return run(std::cin);
    }

    int taskmanager::run(std::istream& input)
    {
        int ret = 0;
        std::string line;

        try
        {
            while (std::getline(input, line))
            {
                if (this->add_task(line) != 0)
                {
                    std::cerr << "Error processing command: " << line << '\n';
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Fatal error: " << e.what() << '\n';
            ret = -1;
        }

        return ret;
    }

    taskmanager::~taskmanager() noexcept
    {
        if (!taskmanager::is_dynamic_block_active())
        {
            process_tasks(static_block_task);
        }
    }
} // namespace bulk
