#ifndef _MSC_VER
#include <algorithm>
#endif
#include <charconv>
#include <chrono>
#include <cstddef> // std::size_t
#include <cstdint> // std::uint16_t
#include <exception> // std::exception
#include <filesystem>
#include <fstream>
#include <memory> // std::unique_ptr
#include <optional> // std::optional
#include <ranges>
#include <regex>
#include <sstream> // std::stringstream
#include <string_view> // std::string_view
#include <system_error> // std::errc
#include <thread> // std::thread
#include <unordered_set>
#include <utility> // std::move
#include <vector> // std::vector

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <capture.hpp>
#include <server.hpp>
#include <socket_wrapper.hpp>

namespace
{
    using namespace std::chrono_literals;

    constexpr int COMMAND_COUNT = 10;
    constexpr int TOTAL_COMMANDS = 20;
    constexpr auto SERVER_STARTUP_SLEEP_DURATION{100ms}; // NOLINT(misc-include-cleaner)
    constexpr auto THREAD_SLEEP_DURATION{500ms}; // NOLINT(misc-include-cleaner)
    constexpr std::uint16_t DEFAULT_PORT = 9000;

    void run_client_task(
        const std::string_view cmds,
        const std::uint16_t    port)
    {
        try
        {
            const test_util::ClientSocket client("127.0.0.1", port);

            client.send_data(cmds);
        }
        catch (const std::exception& e)
        {
            FAIL() << "Client task failed with exception: " << e.what();
        }
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
            if (  !entry.is_regular_file()
               || !filename.starts_with("bulk")
               || !filename.ends_with(".log"))
            {
                continue;
            }

            const std::size_t first_underscore = filename.find('_');
            if (first_underscore == std::string::npos)
            {
                continue;
            }

            const std::string timestamp_str = filename.substr(4, first_underscore - 4);
            const std::string_view timestamp_sv = timestamp_str;
            std::int64_t timestamp_val = 0;
            if (std::from_chars(timestamp_sv.data(),
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                timestamp_sv.data() + timestamp_sv.size(), timestamp_val,
                BASE).ec != std::errc{})
            {
                continue;
            }

            if (start_time && end_time)
            {
                // NOLINTNEXTLINE(misc-include-cleaner)
                auto start_seconds = std::chrono::floor<std::chrono::seconds>(
                        start_time->time_since_epoch()).count();
                auto end_seconds = std::chrono::floor<std::chrono::seconds>(
                        end_time->time_since_epoch()).count();

                if (timestamp_val >= start_seconds && timestamp_val <= end_seconds)
                {
                    log_files.emplace_back(entry.path());
                }
            }
            else
            {
                log_files.emplace_back(entry.path());
            }
        }
        return log_files;
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

    class HW10 : public ::testing::Test
    {
    public:
        HW10() = default;
        ~HW10() override = default;
        HW10(const HW10&) = delete;
        HW10(HW10&&) = delete;
        HW10& operator=(const HW10&) = delete;
        HW10& operator=(HW10&&) = delete;

    protected:
        static void SetUpTestSuite()
        {
            clear_log_files();
        }

        void SetUp() override
        {
            start_time_ = std::chrono::system_clock::now();

            port_ = DEFAULT_PORT;
            bulk_size_ = 3;

            server_ = std::make_unique<async::Server>(port_, bulk_size_);
            server_thread_ = std::thread([this]()
            {
                try
                {
                    server_->run();
                }
                catch (const std::exception& e)
                {
                    server_exception_ = std::make_exception_ptr(e);
                }
            });

            std::this_thread::sleep_for(SERVER_STARTUP_SLEEP_DURATION);
        }

        void TearDown() override
        {
            if (server_)
            {
                server_->stop();
            }

            if (server_thread_.joinable())
            {
                server_thread_.join();
            }

            if (!HasFailure())
            {
                clear_log_files(start_time_, std::chrono::system_clock::now());
            }

            if (server_exception_)
            {
                try
                {
                    std::rethrow_exception(server_exception_);
                }
                catch(const std::exception& e)
                {
                    FAIL() << "Server thread threw exception: " << e.what();
                }
            }
        }

        [[nodiscard]] auto get_port() const
        {
            return port_;
        }

    private:
        std::uint16_t port_{};
        std::size_t bulk_size_{};
        std::thread server_thread_;
        std::unique_ptr<async::Server> server_;
        std::exception_ptr server_exception_;
        std::chrono::system_clock::time_point start_time_;
    };
} // namespace

TEST_F(HW10, CombinedConnectionTest)
{
    try
    {
        auto range = std::views::iota(0, TOTAL_COMMANDS);
        const auto single_conn_start_time = std::chrono::system_clock::now();
        const std::regex num_regex("(\\d+)");
        const std::unordered_set<int> expected_nums(range.begin(), range.end());
        std::string all_logs_content;
        std::stringstream commands_stream;
        std::stringstream commands1_stream;
        std::stringstream commands2_stream;
        std::unordered_set<int> received_nums;
        std::unordered_set<int> received_nums_from_stdout;
        std::vector<std::string> file_contents;

        received_nums.reserve(COMMAND_COUNT);
        received_nums_from_stdout.reserve(COMMAND_COUNT);

        StdoutCapture::Begin();

        for (int i = 0; i < COMMAND_COUNT; i++)
        {
            commands_stream << i << '\n';
        }

        run_client_task(commands_stream.str(), get_port());

        std::this_thread::sleep_for(THREAD_SLEEP_DURATION);

        const std::string output = StdoutCapture::End();
        ASSERT_THAT(output, testing::AllOf(
            testing::HasSubstr("bulk: 0, 1, 2"),
            testing::HasSubstr("bulk: 3, 4, 5"),
            testing::HasSubstr("bulk: 6, 7, 8"),
            testing::HasSubstr("bulk: 9")));

        for (const auto& path :
                get_log_files(single_conn_start_time, std::chrono::system_clock::now()))
        {
            const std::ifstream file(path);
            std::stringstream buffer;

            buffer << file.rdbuf();
            file_contents.emplace_back(buffer.str());
        }

        ASSERT_EQ(file_contents.size(), 4);
        ASSERT_THAT(file_contents, testing::UnorderedElementsAre(
            "bulk: 0, 1, 2\n",
            "bulk: 3, 4, 5\n",
            "bulk: 6, 7, 8\n",
            "bulk: 9\n"));

        std::this_thread::sleep_for(1s); // NOLINT(misc-include-cleaner)

        const auto two_conn_start_time = std::chrono::system_clock::now();

        StdoutCapture::Begin();

        for (int i = 0; i < COMMAND_COUNT; i++)
        {
            commands1_stream << i << '\n';
        }

        const std::string commands1 = commands1_stream.str();

        for (int i = COMMAND_COUNT; i < TOTAL_COMMANDS; i++)
        {
            commands2_stream << i << '\n';
        }

        const std::string commands2 = commands2_stream.str();

        {
            const std::jthread client1(run_client_task, commands1, get_port());
            const std::jthread client2(run_client_task, commands2, get_port());
        }

        std::this_thread::sleep_for(THREAD_SLEEP_DURATION);
        const std::string two_conn_output = StdoutCapture::End();

        auto log_files = get_log_files(two_conn_start_time,
            std::chrono::system_clock::now());
        ASSERT_EQ(log_files.size(), 7);

        for (const auto& path : log_files)
        {
            const std::ifstream file(path);
            std::stringstream buffer;

            buffer << file.rdbuf();
            all_logs_content += buffer.str();
        }

        auto words_begin = std::sregex_iterator(all_logs_content.cbegin(),
            all_logs_content.cend(), num_regex);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; i++)
        {
            const std::smatch& match = *i;
            const char *const first = std::to_address(match.begin()->first);
            const char *const last  = std::to_address(match.begin()->second);
            int value{};

            if (std::from_chars(first, last, value).ec == std::errc{})
            {
                received_nums.insert(value);
            }
        }

        ASSERT_EQ(received_nums, expected_nums);

        auto words_begin_stdout = std::sregex_iterator(two_conn_output.cbegin(),
            two_conn_output.cend(), num_regex);
        auto words_end_stdout = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin_stdout; i != words_end_stdout; i++)
        {
            const std::smatch& match = *i;
            const char *const first = std::to_address(match.begin()->first);
            const char *const last  = std::to_address(match.begin()->second);
            int value{};

            if (std::from_chars(first, last, value).ec == std::errc{})
            {
                received_nums_from_stdout.insert(value);
            }
        }

        ASSERT_EQ(received_nums_from_stdout, expected_nums);
    }
    catch (const std::exception& e)
    {
        FAIL() << "Test failed with exception: " << e.what();
    }
}

TEST_F(HW10, DynamicBlockTest)
{
    try
    {
        const auto test_start_time = std::chrono::system_clock::now();
        const std::string commands1 =
            "cmd1_1\n"
            "cmd1_2\n"
            "{\n"
            "dyn1_1\n"
            "dyn1_2\n"
            "dyn1_3\n"
            "}\n"
            "cmd1_3\n";
        const std::string commands2 =
            "cmd2_1\n"
            "cmd2_2\n"
            "{\n"
            "dyn2_1\n"
            "dyn2_2\n"
            "}\n"
            "cmd2_3\n";
        const std::string dynamic_block1 = "bulk: dyn1_1, dyn1_2, dyn1_3\n";
        const std::string dynamic_block2 = "bulk: dyn2_1, dyn2_2\n";
        const std::unordered_set<std::string> expected_static_cmds =
        {
            "cmd1_1", "cmd1_2", "cmd1_3", "cmd2_1", "cmd2_2", "cmd2_3"
        };
        std::string static_content;
        std::string word;
        std::unordered_set<std::string> received_static_cmds;
        std::vector<std::string> file_contents;

        received_static_cmds.reserve(expected_static_cmds.size());

        {
            const std::jthread client1(run_client_task, commands1, get_port());
            const std::jthread client2(run_client_task, commands2, get_port());
        }

        std::this_thread::sleep_for(THREAD_SLEEP_DURATION);

        auto log_files = get_log_files(test_start_time, std::chrono::system_clock::now());

        ASSERT_GE(log_files.size(), 4);

        for (const auto& path : log_files)
        {
            const std::ifstream file(path);
            std::stringstream buffer;

            buffer << file.rdbuf();
            file_contents.emplace_back(buffer.str());
        }

        const auto it1 = std::ranges::find(file_contents, dynamic_block1);
        ASSERT_NE(it1, file_contents.end());
        file_contents.erase(it1);

        const auto it2 = std::ranges::find(file_contents, dynamic_block2);
        ASSERT_NE(it2, file_contents.end());
        file_contents.erase(it2);

        ASSERT_GE(file_contents.size(), 2);

        for (const auto& block : file_contents)
        {
            static_content += block;
        }

        std::stringstream static_content_stream(static_content);

        while (static_content_stream >> word)
        {
            if (word != "bulk:")
            {
                std::erase(word, ',');
                received_static_cmds.insert(std::move(word));
            }
        }

        ASSERT_EQ(received_static_cmds, expected_static_cmds);
    }
    catch (const std::exception& e)
    {
        FAIL() << "Test failed with exception: " << e.what();
    }
}
