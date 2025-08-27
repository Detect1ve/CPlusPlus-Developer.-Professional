#if __GNUC__ < 14
#include <charconv>
#endif
#include <chrono>
#include <filesystem>

#include <gtest/gtest.h>

#include <capture.hpp>
#include <taskmanager.hpp>

namespace
{
    const std::string& TASK_MANAGER_NAME()
    {
        static const std::string s_name = "bulk";

        return s_name;
    }

    std::vector<std::filesystem::path> get_log_files(
        std::optional<std::chrono::system_clock::time_point> start_time,
        std::optional<std::chrono::system_clock::time_point> end_time)
    {
        constexpr unsigned char BASE = 10;
        std::vector<std::filesystem::path> log_files;

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

                const std::string_view timestamp_sv = timestamp_str;
                std::int64_t timestamp_seconds = 0;
                auto [ptr, ec] = std::from_chars(timestamp_sv.data(),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    timestamp_sv.data() + timestamp_sv.size(), timestamp_seconds, BASE);
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
                        log_files.emplace_back(entry.path());
                    }
                }
                else
                {
                    log_files.emplace_back(entry.path());
                }
            }
        }

        return log_files;
    }

    void checkLogFile(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time,
        const std::size_t                            expected_files)
    {
        ASSERT_EQ(get_log_files(start_time, end_time).size(), expected_files);
    }

    void clear_log_files(
        std::optional<std::chrono::system_clock::time_point> start_time,
        std::optional<std::chrono::system_clock::time_point> end_time)
    {
        for (const auto& path : get_log_files(start_time, end_time))
        {
            std::filesystem::remove(path);
        }
    }

    void clear_log_files()
    {
        clear_log_files(std::nullopt, std::nullopt);
    }
} // namespace

class HW7 : public ::testing::Test
{
    std::chrono::system_clock::time_point start_time_;
protected:
    void SetUp() override
    {
        clear_log_files();
        start_time_ = std::chrono::system_clock::now();
    }

    void TearDown() override
    {
        clear_log_files(start_time_, std::chrono::system_clock::now());
    }
};

TEST_F(HW7, StaticBlocks)
{
    const auto start_time = std::chrono::system_clock::now();
    StdoutCapture::Begin();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME());
        int ret = 0;

        const std::string input_data1 =
            "cmd1\n"
            "cmd2\n"
            "cmd3\n";
        std::istringstream iss1(input_data1);

        ret = my_task_manager.run(iss1);
        if (ret != 0)
        {
            std::cerr << "run return " << ret << '\n';
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        const std::string input_data2 =
            "cmd4\n"
            "cmd5\n"
            ; // EOF
        std::istringstream iss2(input_data2);

        ret = my_task_manager.run(iss2);
        if (ret != 0)
        {
            std::cerr << "run return " << ret << '\n';
        }
    }

    auto capturedStdout = StdoutCapture::End();

    const std::string expectedOutput =
        TASK_MANAGER_NAME() + ": cmd1, cmd2, cmd3\n" +
        TASK_MANAGER_NAME() + ": cmd4, cmd5\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    checkLogFile(start_time, std::chrono::system_clock::now(), 2);
}

TEST_F(HW7, DynamicBlocks)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const auto start_time = std::chrono::system_clock::now();

    StdoutCapture::Begin();
    {
        bulk::taskmanager my_task_manager(3, TASK_MANAGER_NAME());
        int ret = 0;

        const std::string input_data1 =
            "cmd1\n"
            "cmd2\n";
        std::istringstream iss1(input_data1);
        ret = my_task_manager.run(iss1);
        ASSERT_EQ(ret, 0);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        const std::string input_data2 =
            "{\n"
            "cmd3\n"
            "cmd4\n"
            "}\n";
        std::istringstream iss2(input_data2);
        ret = my_task_manager.run(iss2);
        ASSERT_EQ(ret, 0);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        const std::string input_data3 =
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
            "cmd11\n";
        std::istringstream iss3(input_data3);
        ret = my_task_manager.run(iss3);
        ASSERT_EQ(ret, 0);
    }

    auto capturedStdout = StdoutCapture::End();

    const std::string expectedOutput =
        TASK_MANAGER_NAME() + ": cmd1, cmd2\n" +
        TASK_MANAGER_NAME() + ": cmd3, cmd4\n" +
        TASK_MANAGER_NAME() + ": cmd5, cmd6, cmd7, cmd8, cmd9\n";

    ASSERT_EQ(capturedStdout, expectedOutput);

    checkLogFile(start_time, std::chrono::system_clock::now(), 3);
}
