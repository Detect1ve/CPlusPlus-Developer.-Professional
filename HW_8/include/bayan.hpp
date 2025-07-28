#ifndef BAYAN_HPP
#define BAYAN_HPP

#include <boost/filesystem.hpp>

#include <fstream>
#include <regex>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)\
 || defined(__clang__)\
 || __GNUC__ < 14
#include <span>
#endif

std::string compute_crc32(std::string_view input);
std::string compute_md5(std::string_view input);

#define string_and_enum(algorithm) {#algorithm, algorithm}
#define enum_and_func(algorithm) {algorithm, compute_##algorithm}

enum hash_algorithm : std::uint8_t
{
    crc32,
    md5,
};

struct HashAlgorithm
{
    HashAlgorithm();
    explicit HashAlgorithm(enum hash_algorithm value);
    explicit HashAlgorithm(std::string_view name);

    [[nodiscard]] std::string compute_hash(std::string_view input) const;

private:
    enum hash_algorithm value;
    std::function<std::string(std::string_view)> hash_function;

    std::unordered_map<std::string, enum hash_algorithm> name_to_enum_map =
    {
        string_and_enum(crc32),
        string_and_enum(md5)
    };

    std::unordered_map<enum hash_algorithm, std::function<std::string(std::string_view)>>
        enum_to_func_map =
        {
            enum_and_func(crc32),
            enum_and_func(md5)
        };
};

struct FileInfo
{
    FileInfo(
        boost::filesystem::path path,
        uintmax_t               size,
        std::size_t             block_size);

    FileInfo(const FileInfo& other);
    FileInfo(FileInfo&&) noexcept = default;
    FileInfo& operator=(const FileInfo& other);
    FileInfo& operator=(FileInfo&&) noexcept = default;

    ~FileInfo() noexcept;

    [[nodiscard]] uintmax_t get_size() const;
    [[nodiscard]] const std::vector<std::string>& get_hashes() const;
    boost::filesystem::path get_path() const;

    std::string compute_block_hash(
        std::size_t          block_index,
        const HashAlgorithm& hash_algo) const;

private:
    void open_file() const;
    void close_file() const;

    mutable std::ifstream file_stream;
    boost::filesystem::path path;
    mutable std::vector<std::string> hashes;
    uintmax_t size;
    std::size_t block_size_;
    mutable bool file_opened{false};
};

struct BlockSize
{
    int value;
};

struct MinFileSize
{
    int value;
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

class FileScanner {
public:
    FileScanner(
        bool               scan_level,
        BlockSize          block_size,
        MinFileSize        min_file_size,
        const ExcludeDirs& exclude_dirs,
        const FileMasks&   file_masks,
        const ScanDirs&    scan_dirs);

    std::vector<FileInfo> scan_directories();

private:
    void scan_directory_recursive(
        const boost::filesystem::path& dir,
        std::vector<FileInfo>&              files);
    void scan_directory_non_recursive(
        const boost::filesystem::path& dir,
        std::vector<FileInfo>&              files);
    [[nodiscard]] bool matches_masks(const boost::filesystem::path& file_path) const;
    [[nodiscard]] bool is_excluded(const boost::filesystem::path& dir) const;

    bool scan_level_;
    int block_size_;
    uintmax_t min_file_size_;
    std::vector<std::regex> mask_regexes_;
    std::vector<std::string> exclude_dirs_;
    std::vector<std::string> file_masks_;
    std::vector<std::string> scan_dirs_;
};

class DuplicateFinder
{
public:
    DuplicateFinder(
        HashAlgorithm hash_algo,
        int           block_size);

    std::vector<std::vector<FileInfo>> find_duplicates(std::vector<FileInfo>& files);

private:
    std::vector<std::vector<FileInfo>>
        find_duplicates_in_group(std::vector<FileInfo*>& files);

    HashAlgorithm hash_algo_;
    int block_size_;
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
    HashAlgorithm hash_algorithm;
    std::vector<std::string> exclude_dirs;
    std::vector<std::string> file_masks;
    std::vector<std::string> scan_dirs;
    int block_size{};
    int min_file_size{};
    bool scan_level{};
};

std::pair<ProcessStatus, Options> option_process(std::span<const char *const> argv);

ProcessStatus process_files(const Options& options);

#endif /* BAYAN_HPP */
