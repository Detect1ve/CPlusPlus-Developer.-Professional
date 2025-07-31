#if __GNUC__ < 14
#include <charconv>
#endif
#include <chrono>
#include <filesystem>

#include <gtest/gtest.h>

#include <taskmanager.hpp>

namespace
{
    enum : unsigned char
    {
        BASE = 10,
    };

    const std::string& TASK_MANAGER_NAME()
    {
        static const std::string s_name = "bulk";

        return s_name;
    }

    void checkLogFile(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time)
    {
        bool foundLogFile = false;

        for (const auto& entry : std::filesystem::directory_iterator("."))
        {
            const std::string filename = entry.path().filename().string();
            if (filename.starts_with(TASK_MANAGER_NAME()) && filename.ends_with(".log"))
            {
                const std::string timestamp_str =
                    filename.substr(TASK_MANAGER_NAME().size(),
                                    filename.find(".log") - TASK_MANAGER_NAME().size());
                int64_t timestamp_seconds = 0;

                auto [ptr, ec] = std::from_chars(timestamp_str.data(),
                    std::next(timestamp_str.data(),
                        static_cast<std::string::difference_type>(timestamp_str.size())),
                    timestamp_seconds, BASE);
                ASSERT_EQ(ec, std::errc{});

                auto start_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                    start_time.time_since_epoch()).count();
                auto end_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                    end_time.time_since_epoch()).count();

                if (  timestamp_seconds >= start_seconds
                   && timestamp_seconds <= end_seconds)
                {
                    foundLogFile = true;
                    break;
                }
            }
        }

        auto start_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            start_time.time_since_epoch()).count();
        auto end_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            end_time.time_since_epoch()).count();

        ASSERT_TRUE(foundLogFile) << "Log file not found in time range [" << start_seconds
            << ", " << end_seconds << "]";
    }

    void clear_log_files(
        std::optional<std::chrono::system_clock::time_point> start_time,
        std::optional<std::chrono::system_clock::time_point> end_time)
    {
        for (const auto &entry : std::filesystem::directory_iterator("."))
        {
            const std::string filename = entry.path().filename().string();
            if (  entry.is_regular_file()
               && filename.starts_with(TASK_MANAGER_NAME())
               && filename.ends_with(".log"))
            {
                const std::string timestamp_str =
                    filename.substr(TASK_MANAGER_NAME().size(),
                    filename.find(".log") - TASK_MANAGER_NAME().size());
                if (timestamp_str.empty())
                {
                    continue;
                }

                int64_t timestamp_seconds = 0;
                auto [ptr, ec] = std::from_chars(timestamp_str.data(),
                    std::next(timestamp_str.data(),
                        static_cast<std::string::difference_type>(timestamp_str.size())),
                    timestamp_seconds, BASE);
                if (ec != std::errc())
                {
                    continue;
                }

                if (start_time && end_time)
                {
                    auto start_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                            start_time->time_since_epoch()).count();
                    auto end_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                            end_time->time_since_epoch()).count();

                    if (  timestamp_seconds >= start_seconds
                       && timestamp_seconds <= end_seconds)
                    {
                        std::filesystem::remove(entry.path());
                    }
                }
                else
                {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    }

    void clear_log_files()
    {
        clear_log_files(std::nullopt, std::nullopt);
    }
} // namespace

class HW7 : public ::testing::Test
{
std::chrono::system_clock::time_point start_time;

protected:
    void SetUp() override
    {
        clear_log_files();
        start_time = std::chrono::system_clock::now();
    }

    void TearDown() override
    {
        clear_log_files(start_time, std::chrono::system_clock::now());
    }

public:
    [[nodiscard]] std::chrono::system_clock::time_point get_start_time() const
    {
        return start_time;
    }
};

TEST_F(HW7, StaticBlocks)
{
    testing::internal::CaptureStdout();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME());
        int ret = 0;

        const std::string input_data =
            "cmd1\n"
            "cmd2\n"
            "cmd3\n"
            "cmd4\n"
            "cmd5\n"
            ; // EOF
        std::istringstream iss(input_data);

        ret = my_task_manager.run(iss);
        if (ret != 0)
        {
            std::cerr << "run return " << ret << '\n';
        }
    }

    auto capturedStdout = testing::internal::GetCapturedStdout();

    const auto end_time = std::chrono::system_clock::now();

    const std::string expectedOutput =
        TASK_MANAGER_NAME() + ": cmd1, cmd2, cmd3\n" +
        TASK_MANAGER_NAME() + ": cmd4, cmd5\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    checkLogFile(get_start_time(), end_time);
}

TEST_F(HW7, DynamicBlocks)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));

    testing::internal::CaptureStdout();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME());
        int ret = 0;

        const std::string input_data =
            "cmd1\n"
            "cmd2\n"
            "{\n"
            "cmd3\n"
            "cmd4\n"
            "}\n"
            "{\n"
            "cmd5\n"
            "cmd6\n"
            "{\n"
            "cmd7\n"
            "cmd8\n"
            "}\n"
            "cmd9\n"
            "}\n"
            "{\n"
            "cmd10\n"
            "cmd11\n"
            ; // EOF
        std::istringstream iss(input_data);

        ret = my_task_manager.run(iss);
        if (ret != 0)
        {
            std::cerr << "run return " << ret << '\n';
        }
    }

    auto capturedStdout = testing::internal::GetCapturedStdout();

    const auto end_time = std::chrono::system_clock::now();

    const std::string expectedOutput =
        TASK_MANAGER_NAME() + ": cmd1, cmd2\n" +
        TASK_MANAGER_NAME() + ": cmd3, cmd4\n" +
        TASK_MANAGER_NAME() + ": cmd5, cmd6, cmd7, cmd8, cmd9\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    checkLogFile(get_start_time(), end_time);
}
