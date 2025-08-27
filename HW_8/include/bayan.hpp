#ifndef BAYAN_HPP
#define BAYAN_HPP

#if defined(__clang__)\
 || __GNUC__ < 14
#include <cstdint>
#endif
#include <fstream>
#include <regex>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)\
 || defined(__clang__)\
 || __GNUC__ < 14
#include <span>
#endif
#include <unordered_map>

std::string compute_crc32(std::string_view input);
std::string compute_md5(std::string_view input);

enum hash_algorithm : std::uint8_t
{
    crc32,
    md5,
};

struct HashAlgorithm
{
    HashAlgorithm();
    explicit HashAlgorithm(hash_algorithm value);
    explicit HashAlgorithm(std::string_view name);

    [[nodiscard]] std::string compute_hash(std::string_view input) const;

private:
    hash_algorithm value_;
    std::function<std::string(std::string_view)> hash_function;

    static
    std::function<std::string(std::string_view)> get_hash_function(hash_algorithm value);

    std::unordered_map<std::string, hash_algorithm> name_to_enum_map =
    {
        {"crc32", crc32},
        {"md5", md5}
    };
};

struct BlockSize
{
    uintmax_t value;
};

struct MinFileSize
{
    uintmax_t value;
};

struct ExcludeDirs
{
    std::vector<std::string> value;
};

struct FileMasks
{
    std::vector<std::string> value;
};

struct ScanDirs
{
    std::vector<std::string> value;
};

enum class ProcessStatus : std::uint8_t
{
    SUCCESS = 0,
    HELP_REQUESTED = 1,
    OPTION_ERROR = 2,
    FILE_ERROR = 3,
};

struct Options
{
    bool scan_level{};
    HashAlgorithm hash_algorithm;
    std::vector<std::string> exclude_dirs;
    std::vector<std::string> file_masks;
    std::vector<std::string> scan_dirs;
    uintmax_t block_size{};
    uintmax_t min_file_size{};
};

std::pair<ProcessStatus, Options> option_process(std::span<const char *const> argv);

ProcessStatus process_files(const Options& options);

#endif // BAYAN_HPP
