#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <array>
#endif

#include <gtest/gtest.h>

#include <absl_strings_match.hpp>
#include <bayan.hpp>
#include <capture.hpp>
#include <wrapper_boost_filesystem.hpp>

TEST(HW8, NoDuplicatesTest)
{
    const boost::filesystem::path temp_dir =
        boost::filesystem::current_path() / "bayan_test_dir";
    const boost::filesystem::path cpp_file = temp_dir / "cpp.txt";
    const boost::filesystem::path world_file = temp_dir / "world.txt";
    const std::string& temp_dir_str = temp_dir.string();
    const std::array argv =
    {
        "bayan_test",
        "--scan_dirs", temp_dir_str.c_str(),
        "--block_size", "5",
        "--scan_level", "0",
        "--file_masks", "*.txt"
    };
    auto result = ProcessStatus::SUCCESS;

    if (boost::filesystem::exists(temp_dir))
    {
        boost::filesystem::remove_all(temp_dir);
    }

    boost::filesystem::create_directory(temp_dir);

    {
        std::ofstream cpp_stream(cpp_file.string());
        cpp_stream << "Hello, C++\n";
        cpp_stream.close();

        std::ofstream world_stream(world_file.string());
        world_stream << "Hello, World\n";
        world_stream.close();
    }

    ASSERT_TRUE(boost::filesystem::exists(cpp_file));
    ASSERT_TRUE(boost::filesystem::exists(world_file));

    StdoutCapture::Begin();

    auto [status, options] = option_process(argv);
    ASSERT_EQ(status, ProcessStatus::SUCCESS);

    result = process_files(options);

    auto capturedStdout = StdoutCapture::End();

    ASSERT_EQ(result, ProcessStatus::SUCCESS);
    ASSERT_EQ(capturedStdout, "No duplicate files found.\n");

    boost::filesystem::remove_all(temp_dir);
}

TEST(HW8, FindDuplicatesTest)
{
    const boost::filesystem::path current_dir = boost::filesystem::current_path();
    const boost::filesystem::path temp_dir = current_dir / "bayan_test_duplicates";
    const std::string& temp_dir_str = temp_dir.string();

    const boost::filesystem::path level1_dir = temp_dir / "level1";
    const boost::filesystem::path level2_dir = level1_dir / "level2";
    const std::string& level2_dir_str = level2_dir.string();

    const boost::filesystem::path cpp_file1 = temp_dir / "cpp1.txt";
    const boost::filesystem::path cpp_file2 = temp_dir / "cpp2.txt";

    const boost::filesystem::path world_file1 = temp_dir / "world1.txt";
    const boost::filesystem::path world_file2 = level1_dir / "world2.txt";
    const boost::filesystem::path world_file3 = level2_dir / "world3.txt";
    const std::array argv =
    {
        "bayan_test",
        "--scan_dirs", temp_dir_str.c_str(),
        "--scan_dirs", level2_dir_str.c_str(),
        "--block_size", "5",
        "--scan_level", "1",
        "--file_masks", "*TXT",
        "--hash_algorithm", "md5"
    };
    auto result = ProcessStatus::SUCCESS;

    if (boost::filesystem::exists(temp_dir))
    {
        boost::filesystem::remove_all(temp_dir);
    }

    boost::filesystem::create_directory(temp_dir);

    boost::filesystem::create_directory(level1_dir);
    boost::filesystem::create_directory(level2_dir);

    {
        std::ofstream cpp_stream1(cpp_file1.string());
        cpp_stream1 << "Hello, C++\n";
        cpp_stream1.close();

        std::ofstream cpp_stream2(cpp_file2.string());
        cpp_stream2 << "Hello, C++\n";
        cpp_stream2.close();

        std::ofstream world_stream1(world_file1.string());
        world_stream1 << "Hello, World\n";
        world_stream1.close();

        std::ofstream world_stream2(world_file2.string());
        world_stream2 << "Hello, World\n";
        world_stream2.close();

        std::ofstream world_stream3(world_file3.string());
        world_stream3 << "Hello, World\n";
        world_stream3.close();
    }

    ASSERT_TRUE(boost::filesystem::exists(cpp_file1));
    ASSERT_TRUE(boost::filesystem::exists(cpp_file2));
    ASSERT_TRUE(boost::filesystem::exists(world_file1));
    ASSERT_TRUE(boost::filesystem::exists(world_file2));
    ASSERT_TRUE(boost::filesystem::exists(world_file3));

    StdoutCapture::Begin();

    auto [status, options] = option_process(argv);
    ASSERT_EQ(status, ProcessStatus::SUCCESS);

    result = process_files(options);

    auto capturedStdout = StdoutCapture::End();

    ASSERT_EQ(result, ProcessStatus::SUCCESS);

    ASSERT_FALSE(absl::StrContains(capturedStdout, "No duplicate files found.\n"));

    ASSERT_TRUE(capturedStdout.find(cpp_file1.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(cpp_file2.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file1.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file2.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file3.string()) != std::string::npos);

    boost::filesystem::remove_all(temp_dir);
}

TEST(HW8, UnrecognisedOptionTest)
{
    const std::array argv =
    {
        "bayan_test",
        "--dummy-option"
    };

    StderrCapture::Begin();
    auto [status, options] = option_process(argv);
    auto capturedStderr = StderrCapture::End();

    ASSERT_EQ(status, ProcessStatus::OPTION_ERROR);
    ASSERT_TRUE(absl::StrContains(capturedStderr, "unrecognised option"));
}

TEST(HW8, MissingMandatoryOptionTest)
{
    const std::array argv =
    {
        "bayan_test",
        "--scan_dirs", "/tmp",
        "--scan_level", "0"
    };

    StderrCapture::Begin();
    auto [status, options] = option_process(argv);
    auto capturedStderr = StderrCapture::End();

    ASSERT_EQ(status, ProcessStatus::OPTION_ERROR);
    ASSERT_TRUE(absl::StrContains(capturedStderr, "required"));
}

TEST(HW8, BadValueOptionTest)
{
    const std::array argv =
    {
        "bayan_test",
        "--scan_dirs", "/tmp",
        "--scan_level", "0",
        "--block_size", "-1"
    };

    StderrCapture::Begin();
    auto [status, options] = option_process(argv);
    auto capturedStderr = StderrCapture::End();

    ASSERT_EQ(status, ProcessStatus::OPTION_ERROR);
    ASSERT_TRUE(absl::StrContains(capturedStderr, "non-negative"));
}

TEST(HW8, BadHashAlgorithmTest)
{
    const std::array argv =
    {
        "bayan_test",
        "--scan_dirs", "/tmp",
        "--scan_level", "0",
        "--block_size", "128",
        "--hash_algorithm", "dummy_algorithm"
    };

    StderrCapture::Begin();
    auto [status, options] = option_process(argv);
    auto capturedStderr = StderrCapture::End();

    ASSERT_EQ(status, ProcessStatus::OPTION_ERROR);
    ASSERT_TRUE(absl::StrContains(capturedStderr, "is invalid"));
}
