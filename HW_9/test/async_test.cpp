#if defined(__clang__)\
 || __GNUC__ < 14\
 || __cplusplus <=  202002L
#include <charconv>
#endif
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include <absl/strings/match.h>

#include <async.h>

namespace
{
    void clear_log_files(
        std::optional<std::chrono::system_clock::time_point> start_time,
        std::optional<std::chrono::system_clock::time_point> end_time)
    {
        enum : unsigned char
        {
            BASE = 10
        };

        for (const auto &entry : std::filesystem::directory_iterator("."))
        {
            const std::string filename = entry.path().filename().string();
            if (  entry.is_regular_file()
               && filename.starts_with("bulk")
               && filename.ends_with(".log"))
            {
                const std::size_t first_underscore = filename.find('_');
                if (first_underscore == std::string::npos)
                {
                    continue;
                }

                const std::string timestamp_str =
                    filename.substr(4, first_underscore - 4);
                int64_t timestamp_val = 0;
                auto [ptr, ec] = std::from_chars(timestamp_str.data(),
                    std::next(timestamp_str.data(),
                        static_cast<std::string::difference_type>(timestamp_str.size())),
                    timestamp_val, BASE);
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

                    if (  timestamp_val >= start_seconds
                        && timestamp_val <= end_seconds)
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

    std::string read_file_content(const std::filesystem::path &path)
    {
        const std::ifstream file(path);
        std::stringstream buffer;

        buffer << file.rdbuf();

        return buffer.str();
    }
} // namespace

class HW9 : public ::testing::Test
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
};

TEST_F(HW9, MainFunctionality)
{
    testing::internal::CaptureStdout();

    const std::size_t bulk = 5;
    auto *handle1 = async::connect(bulk);
    auto *handle2 = async::connect(bulk);

    async::receive(handle1, "1", 1);
    async::receive(handle2, "1\n", 2);
    async::receive(handle1, "\n2\n3\n4\n5\n6\n{\na\n",
        sizeof("\n2\n3\n4\n5\n6\n{\na\n") - 1);
    async::receive(handle1, "b\nc\nd\n}\n89\n", sizeof("b\nc\nd\n}\n89\n") - 1);
    async::disconnect(handle1);
    async::disconnect(handle2);

    const std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(absl::StrContains(output, "bulk: 1\n"));
    ASSERT_TRUE(absl::StrContains(output, "bulk: 1, 2, 3, 4, 5\n"));
    ASSERT_TRUE(absl::StrContains(output, "bulk: 6\n"));
    ASSERT_TRUE(absl::StrContains(output, "bulk: a, b, c, d\n"));
    ASSERT_TRUE(absl::StrContains(output, "bulk: 89\n"));

    std::vector<std::string> log_contents;
    for (const auto &entry : std::filesystem::directory_iterator("."))
    {
        if (  entry.is_regular_file()
           && entry.path().filename().string().starts_with("bulk")
           && entry.path().extension() == ".log")
        {
            log_contents.emplace_back(read_file_content(entry.path()));
        }
    }

    ASSERT_GE(log_contents.size(), 4);

    bool block1_found = false;
    bool block2_found = false;
    bool block3_found = false;
    bool block4_found = false;
    bool block5_found = false;

    for (const auto &content : log_contents)
    {
        if (absl::StrContains(content, "bulk: 1\n"))
        {
            block1_found = true;
        }
        else if (absl::StrContains(content, "bulk: 1, 2, 3, 4, 5\n"))
        {
            block2_found = true;
        }
        else if (absl::StrContains(content, "bulk: 6\n"))
        {
            block3_found = true;
        }
        else if (absl::StrContains(content, "bulk: a, b, c, d\n"))
        {
            block4_found = true;
        }
        else if (absl::StrContains(content, "bulk: 89\n"))
        {
            block5_found = true;
        }
    }

    ASSERT_TRUE(block1_found);
    ASSERT_TRUE(block2_found);
    ASSERT_TRUE(block3_found);
    ASSERT_TRUE(block4_found);
    ASSERT_TRUE(block5_found);
}
