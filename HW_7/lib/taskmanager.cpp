#if !defined(_WIN32) && !defined(_MSC_VER)
#include <csignal>
#endif
#include <filesystem>
#include <fstream>
#include <ranges>

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

namespace
{
    [[nodiscard]] bulk::taskmanager*& get_task_manager_instance() noexcept
    {
        static bulk::taskmanager* instance = nullptr; // NOLINT(misc-const-correctness)

        return instance;
    }

#if !defined(_WIN32) && !defined(_MSC_VER)
    void signal_handler(const int /*signal*/)
    {
        if (auto* instance = get_task_manager_instance(); instance)
        {
            instance->stop();
        }
    }
#endif
} // namespace

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

    int taskmanager::process_tasks(std::vector<std::string>& block_task)
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
            while (!stop_flag)
            {
                if (std::getline(input, line))
                {
                    if (this->add_task(line) != 0)
                    {
                        std::cerr << "Error processing command: " << line << '\n';
                    }
                }
                else if (input.eof() || input.fail())
                {
                    break;
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

    void taskmanager::stop() noexcept
    {
        stop_flag = true;
    }

    void taskmanager::setup_signal_handling()
    {
        get_task_manager_instance() = this;
#if !defined(_WIN32) && !defined(_MSC_VER)
        struct sigaction sigaction_info {};

        sigaction_info.sa_handler = signal_handler;
        sigemptyset(&sigaction_info.sa_mask);
        sigaction_info.sa_flags = 0;
        if (sigaction(SIGHUP, &sigaction_info, nullptr) == -1)
        {
            std::cerr << "Failed to register SIGHUP handler, but continue anyway\n";
        }

        if (sigaction(SIGINT, &sigaction_info, nullptr) == -1)
        {
            std::cerr << "Failed to register SIGINT handler, but continue anyway\n";
        }

        if (sigaction(SIGQUIT, &sigaction_info, nullptr) == -1)
        {
            std::cerr << "Failed to register SIGQUIT handler, but continue anyway\n";
        }

        if (sigaction(SIGTERM, &sigaction_info, nullptr) == -1)
        {
            std::cerr << "Failed to register SIGTERM handler, but continue anyway\n";
        }
#endif
    }

    taskmanager::~taskmanager() noexcept
    {
        try
        {
            if (!taskmanager::is_dynamic_block_active())
            {
                process_tasks(static_block_task);
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "error in destructor: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "error in destructor: unknown\n";
        }
    }
} // namespace bulk
