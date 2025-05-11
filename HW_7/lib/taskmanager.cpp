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

        template<typename T>
        auto operator<<(const T& value) -> composite_stream&
        {
            s1 << value;
            s2 << value;

            return *this;
        }

        auto operator<<(std::ostream& (*manip)(std::ostream&)) -> composite_stream&
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
    [[nodiscard]] auto taskmanager::is_dynamic_block_active() const -> bool
    {
        return dynamic_block_nesting_level > 0;
    }

    auto taskmanager::add_task(std::string_view task) -> int
    {
        static constexpr std::string_view CLOSE_BRACKET = "}";
        static constexpr std::string_view OPEN_BRACKET = "{";
        int ret = 0;

        if (task == OPEN_BRACKET)
        {
            if (!taskmanager::is_dynamic_block_active())
            {
                dynamic_block_timestamp = time(nullptr);
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
            if (static_block_timestamp == 0)
            {
                static_block_timestamp = time(nullptr);
            }

            static_block_task.emplace_back(task);
            if (++task_count % max_static_task_count == 0)
            {
                ret = process_tasks(static_block_task);
            }
        }

        return ret;
    }

    auto taskmanager::process_tasks(std::vector<std::string>& block_task) -> int
    {
        int ret = 0;

        if (block_task.empty())
        {
            return ret;
        }

        std::ofstream file;
        time_t *timestamp =
            (task_count != 0 ? &static_block_timestamp : &dynamic_block_timestamp);

#ifdef __cpp_lib_format
        file.open(std::format("{}{}.log", task_manager_name, *timestamp));
#else
        file.open(task_manager_name + std::to_string(*timestamp) + ".log");
#endif

        if (!file.is_open())
        {
            std::cerr << "Failed to open file" << std::endl;
            ret = -1;

            return ret;
        }

        bulk::io::composite_stream output(std::cout, file);

        output <<
            task_manager_name.substr(task_manager_name.find_last_of("/\\") + 1) << ": ";
        for (size_t i = 0; i < block_task.size(); ++i)
        {
            output << block_task[i];
            if (i < block_task.size() - 1)
            {
                output << ", ";
            }
        }

        output << std::endl;

        block_task.clear();
        file.close();

        *timestamp = 0;
        task_count = 0;

        return ret;
    }

    auto taskmanager::run(std::istream& input) -> int
    {
        int ret = 0;
        std::string line;

        try
        {
            while (std::getline(input, line))
            {
                if (this->add_task(line) != 0)
                {
                    std::cerr << "Error processing command: " << line << std::endl;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Fatal error: " << e.what() << std::endl;
            ret = -1;
        }

        return ret;
    }

    taskmanager::~taskmanager()
    {
        if (!taskmanager::is_dynamic_block_active())
        {
            process_tasks(static_block_task);
        }
    }
} // namespace bulk
