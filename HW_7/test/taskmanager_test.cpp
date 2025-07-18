#include <filesystem>
#if __GNUC__ < 15
#include <charconv>
#endif
#include <chrono>

#include <gtest/gtest.h>

#include <taskmanager.hpp>

static const std::string TASK_MANAGER_NAME = "bulk";

TEST(HW_7, static_blocks)
{
    bool foundLogFile = false;
    auto const start_time = std::chrono::system_clock::now();
    auto end_time = std::chrono::system_clock::time_point{};

    testing::internal::CaptureStdout();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME);
        int ret = 0;

        std::string input_data =
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
            std::cerr << "run return " << ret << std::endl;
        }
    }

    auto capturedStdout = testing::internal::GetCapturedStdout();

    end_time = std::chrono::system_clock::now();

    std::string expectedOutput =
        TASK_MANAGER_NAME + ": cmd1, cmd2, cmd3\n" +
        TASK_MANAGER_NAME + ": cmd4, cmd5\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    for (const auto& entry : std::filesystem::directory_iterator("."))
    {
        std::string filename = entry.path().filename().string();
        if (filename.starts_with(TASK_MANAGER_NAME) && filename.ends_with(".log"))
        {
            std::string timestamp_str = filename.substr(4, filename.find(".log") - 4);
            long long timestamp_seconds = 0;

            auto [ptr, ec] = std::from_chars(timestamp_str.data(),
                timestamp_str.data() + timestamp_str.size(), timestamp_seconds);
            ASSERT_EQ(ec, std::errc{});

            auto start_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                start_time.time_since_epoch()).count();
            auto end_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                end_time.time_since_epoch()).count();

            if (timestamp_seconds >= start_seconds && timestamp_seconds <= end_seconds)
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

TEST(HW_7, dynamic_blocks)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));

    bool foundLogFile = false;
    auto const start_time = std::chrono::system_clock::now();
    auto end_time = std::chrono::system_clock::time_point{};

    testing::internal::CaptureStdout();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME);
        int ret = 0;

        std::string input_data =
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
            std::cerr << "run return " << ret << std::endl;
        }
    }

    auto capturedStdout = testing::internal::GetCapturedStdout();

    end_time = std::chrono::system_clock::now();

    std::string expectedOutput =
        TASK_MANAGER_NAME + ": cmd1, cmd2\n" +
        TASK_MANAGER_NAME + ": cmd3, cmd4\n" +
        TASK_MANAGER_NAME + ": cmd5, cmd6, cmd7, cmd8, cmd9\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    for (const auto& entry : std::filesystem::directory_iterator("."))
    {
        std::string filename = entry.path().filename().string();
        if (filename.starts_with(TASK_MANAGER_NAME) && filename.ends_with(".log"))
        {
            std::string timestamp_str = filename.substr(4, filename.find(".log") - 4);
            long long timestamp_seconds = 0;

            auto [ptr, ec] = std::from_chars(timestamp_str.data(),
                timestamp_str.data() + timestamp_str.size(), timestamp_seconds);
            ASSERT_EQ(ec, std::errc{});

            auto start_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                start_time.time_since_epoch()).count();
            auto end_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                end_time.time_since_epoch()).count();

            if (timestamp_seconds >= start_seconds && timestamp_seconds <= end_seconds)
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
