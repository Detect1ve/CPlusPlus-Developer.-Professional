#include <gtest/gtest.h>

#include <bayan.hpp>

TEST(BayanTest, NoDuplicatesTest)
{
    boost::filesystem::path temp_dir =
        boost::filesystem::current_path() / "bayan_test_dir";
    boost::filesystem::path cpp_file = temp_dir / "cpp.txt";
    boost::filesystem::path world_file = temp_dir / "world.txt";
    std::string temp_dir_str = temp_dir.string();
    const char* argv[] =
    {
        "bayan_test",
        "--scan_dirs", temp_dir_str.c_str(),
        "--block_size", "5",
        "--scan_level", "0",
        "--file_masks", "*.txt"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    int result = 0;

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

    testing::internal::CaptureStdout();

    auto [ret, options] = option_process(argc, const_cast<char**>(argv));
    ASSERT_EQ(ret, 0);

    result = process_files(options);

    auto capturedStdout = testing::internal::GetCapturedStdout();

    ASSERT_EQ(result, 0);
    ASSERT_EQ(capturedStdout, "No duplicate files found.\n");

    boost::filesystem::remove_all(temp_dir);
}

TEST(BayanTest, FindDuplicatesTest)
{
    boost::filesystem::path current_dir = boost::filesystem::current_path();
    boost::filesystem::path temp_dir = current_dir / "bayan_test_duplicates";
    std::string temp_dir_str = temp_dir.string();

    boost::filesystem::path level1_dir = temp_dir / "level1";
    boost::filesystem::path level2_dir = level1_dir / "level2";
    std::string level2_dir_str = level2_dir.string();

    boost::filesystem::path cpp_file1 = temp_dir / "cpp1.txt";
    boost::filesystem::path cpp_file2 = temp_dir / "cpp2.txt";

    boost::filesystem::path world_file1 = temp_dir / "world1.txt";
    boost::filesystem::path world_file2 = level1_dir / "world2.txt";
    boost::filesystem::path world_file3 = level2_dir / "world3.txt";
    const char* argv[] =
    {
        "bayan_test",
        "--scan_dirs", temp_dir_str.c_str(),
        "--scan_dirs", level2_dir_str.c_str(),
        "--block_size", "5",
        "--scan_level", "1",
        "--file_masks", "*TXT",
        "--hash_algorithm", "md5"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    int result = 0;

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

    testing::internal::CaptureStdout();

    auto [ret, options] = option_process(argc, const_cast<char**>(argv));
    ASSERT_EQ(ret, 0);

    result = process_files(options);

    auto capturedStdout = testing::internal::GetCapturedStdout();

    ASSERT_EQ(result, 0);

    ASSERT_FALSE(capturedStdout.find("No duplicate files found.\n") != std::string::npos);

    ASSERT_TRUE(capturedStdout.find(cpp_file1.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(cpp_file2.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file1.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file2.string()) != std::string::npos);
    ASSERT_TRUE(capturedStdout.find(world_file3.string()) != std::string::npos);

    boost::filesystem::remove_all(temp_dir);
}
