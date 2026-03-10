#ifndef BAYAN_HPP
#define BAYAN_HPP

#if defined(__clang__)\
 || __GNUC__ < 14
#include <cstdint>
#endif
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <functional>
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

enum class hash_algorithm : std::uint8_t
{
    crc32,
    md5,
};

class HashAlgorithm
{
public:
    HashAlgorithm();
    explicit HashAlgorithm(hash_algorithm value);
    explicit HashAlgorithm(std::string_view name);

    [[nodiscard]] std::string compute_hash(std::string_view input) const;

private:
    static
    std::function<std::string(std::string_view)> get_hash_function(hash_algorithm value);

    hash_algorithm value_;
    std::function<std::string(std::string_view)> hash_function_;
    std::unordered_map<std::string, hash_algorithm> name_to_enum_map_ =
    {
        {"crc32", hash_algorithm::crc32},
        {"md5", hash_algorithm::md5}
    };
};

enum class ProcessStatus : std::uint8_t
{
    SUCCESS = 0,
    HELP_REQUESTED = 1,
    OPTION_ERROR = 2,
    FILE_ERROR = 3,
};

struct BlockSize
{
    std::uintmax_t value;
};

struct ExcludeDirs
{
    std::vector<std::string> value;
};

struct FileMasks
{
    std::vector<std::string> value;
};

struct MinFileSize
{
    std::uintmax_t value;
};

struct Options
{
    bool scan_level{};
    HashAlgorithm hash_algorithm;
    std::uintmax_t block_size{};
    std::uintmax_t min_file_size{};
    std::vector<std::string> exclude_dirs;
    std::vector<std::string> file_masks;
    std::vector<std::string> scan_dirs;
};

struct ScanDirs
{
    std::vector<std::string> value;
};

std::pair<ProcessStatus, Options> option_process(std::span<const char *const> argv);

ProcessStatus process_files(const Options& options);

#endif // BAYAN_HPP
