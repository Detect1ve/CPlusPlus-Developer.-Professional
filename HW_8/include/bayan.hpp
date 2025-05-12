#pragma once

#include <boost/filesystem.hpp>

#include <fstream>
#include <regex>

using std::function;
using std::unordered_map;
using std::string_view;
using std::string;
using std::vector;

auto compute_crc32(string_view input) -> string;
auto compute_md5(string_view input) -> string;

#define string_and_enum(algorithm) {#algorithm, algorithm}
#define enum_and_func(algorithm) {algorithm, compute_##algorithm}

typedef enum hash_algorithm
{
    crc32,
    md5,
} hash_algorithm;

struct HashAlgorithm
{
    HashAlgorithm();
    HashAlgorithm(enum hash_algorithm v);
    HashAlgorithm(string_view         s);

    [[nodiscard]] auto compute_hash(string_view input) const -> string;

private:
    enum hash_algorithm value;
    function<string(string_view)> hash_function;

    unordered_map<string, enum hash_algorithm> name_to_enum_map =
    {
        string_and_enum(crc32),
        string_and_enum(md5)
    };

    unordered_map<enum hash_algorithm, function<string(string_view)>> enum_to_func_map =
    {
        enum_and_func(crc32),
        enum_and_func(md5)
    };
};

struct FileInfo
{
    FileInfo(
        const boost::filesystem::path& p,
        uintmax_t                      s,
        size_t                         block_size = 1);

    FileInfo(const FileInfo& other);

    auto operator=(const FileInfo& other) -> FileInfo&;

    ~FileInfo();

    [[nodiscard]] auto get_size() const -> uintmax_t;
    [[nodiscard]] auto get_hashes() const -> const vector<string>&;
    auto get_path() const -> boost::filesystem::path;

    auto compute_block_hash(
        size_t               block_index,
        const HashAlgorithm& hash_algo) const -> string;

private:
    void open_file() const;
    void close_file() const;

    boost::filesystem::path path;
    mutable bool file_opened{false};
    mutable std::ifstream file_stream;
    mutable vector<string> hashes;
    size_t block_size_;
    uintmax_t size;
};

class FileScanner {
public:
    FileScanner(
        bool                  scan_level,
        int                   block_size,
        int                   min_file_size,
        const vector<string>& exclude_dirs,
        const vector<string>& file_masks,
        const vector<string>& scan_dirs);

    auto scan_directories() -> vector<FileInfo>;

private:
    [[nodiscard]] auto matches_masks(const boost::filesystem::path& file_path) const
        -> bool;
    [[nodiscard]] auto is_excluded(const boost::filesystem::path& dir) const -> bool;
    void scan_directory(
        const boost::filesystem::path& dir,
        vector<FileInfo>&              files,
        int                            level);

    bool scan_level_;
    int block_size_;
    uintmax_t min_file_size_;
    vector<std::regex> mask_regexes_;
    vector<string> exclude_dirs_;
    vector<string> file_masks_;
    vector<string> scan_dirs_;
};

class DuplicateFinder
{
public:
    DuplicateFinder(
        const HashAlgorithm& hash_algo,
        int                  block_size);

    auto find_duplicates(vector<FileInfo>& files) -> vector<vector<FileInfo>>;

private:
    auto find_duplicates_in_group(vector<FileInfo*>& files) -> vector<vector<FileInfo>>;

    HashAlgorithm hash_algo_;
    int block_size_;
};

struct Options
{
    bool scan_level;
    HashAlgorithm hash_algorithm;
    int block_size;
    int min_file_size;
    vector<string> exclude_dirs;
    vector<string> file_masks;
    vector<string> scan_dirs;
};

auto option_process(
    int    argc,
    char **argv) -> std::pair<int, Options>;

auto process_files(const Options& options) -> int;
