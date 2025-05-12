#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <fstream>
#include <iostream>
#include <regex>

using boost::filesystem::canonical;
using std::function;
using std::unordered_map;
using std::string;
using std::vector;

auto compute_crc32(const string& input) -> string
{
    boost::crc_32_type result;

    result.process_bytes(input.data(), input.length());

    return std::to_string(result.checksum());
}

auto compute_md5(const string& input) -> string
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;

    hash.process_bytes(input.data(), input.length());
    hash.get_digest(digest);

    const auto *const char_digest = reinterpret_cast<const char*>(&digest);
    string result;

    boost::algorithm::hex(char_digest, char_digest + sizeof(digest),
        std::back_inserter(result));

    return result;
}

#define string_and_enum(algorithm) {#algorithm, algorithm}
#define enum_and_func(algorithm) {algorithm, compute_##algorithm}
typedef enum hash_algorithm
{
    crc32,
    md5,
} hash_algorithm;


struct HashAlgorithm
{

    HashAlgorithm() : value(crc32)
    {
        hash_function = enum_to_func_map.at(value);
    }

    HashAlgorithm(enum hash_algorithm v) : value(v)
    {
        auto it = enum_to_func_map.find(value);

        if (it == enum_to_func_map.end())
        {
            throw std::invalid_argument(
                "Invalid hash algorithm enum value: " + std::to_string(value));
        }

        hash_function = it->second;
    }

    HashAlgorithm(const string& s)
    {
        string lower_s(s);
        std::transform(lower_s.begin(), lower_s.end(), lower_s.begin(),
            [](unsigned char c)
            {
                return std::tolower(c);
            });

        auto it = name_to_enum_map.find(lower_s);

        if (it == name_to_enum_map.cend()) {
            throw boost::program_options::validation_error(
                boost::program_options::validation_error::invalid_option_value,
                "hash_algorithm",
                "Invalid hash algorithm");
        }

        value = it->second;
        hash_function = enum_to_func_map.at(value);
    }

    [[nodiscard]] auto compute_hash(const string& input) const -> string
    {
        return hash_function(input);
    }

private:

    enum hash_algorithm value;
    function<string(const string&)> hash_function;

    unordered_map<string, enum hash_algorithm> name_to_enum_map =
    {
        string_and_enum(crc32),
        string_and_enum(md5)
    };

    unordered_map<enum hash_algorithm, function<string(const string&)>> enum_to_func_map =
    {
        enum_and_func(crc32),
        enum_and_func(md5)
    };
};

struct FileInfo
{
    FileInfo(
        const boost::filesystem::path& p,
        const uintmax_t                s,
        const size_t                   block_size = 1)
        :
        path(p),
        size(s),
        hashes((s + block_size - 1) / block_size) {}

    FileInfo(const FileInfo& other)
        :
        path(other.path),
        size(other.size),
        hashes(other.hashes),
        file_opened(false) {}

    auto operator=(const FileInfo& other) -> FileInfo&
    {
        if (this != &other)
        {
            close_file();

            path = other.path;
            size = other.size;
            hashes = other.hashes;
            file_opened = false;
        }

        return *this;
    }

    ~FileInfo()
    {
        close_file();
    }

    [[nodiscard]] auto get_size() const -> uintmax_t
    {
        return size;
    }

    [[nodiscard]] auto get_hashes() const -> const vector<string>&
    {
        return hashes;
    }

    auto get_path() const -> boost::filesystem::path
    {
        return path;
    }

    auto compute_block_hash(
        const size_t         block_index,
        const size_t         block_size,
        const HashAlgorithm& hash_algo) const -> string
    {
        if (!hashes[block_index].empty())
        {
            return hashes[block_index];
        }

        if (!file_opened)
        {
            open_file();
        }

        file_stream.seekg(block_index * block_size);

        vector<char> buffer(block_size, 0);

        file_stream.read(buffer.data(), block_size);

        std::streamsize bytes_read = file_stream.gcount();

        string block_data(buffer.data(), bytes_read);

        hashes[block_index] = hash_algo.compute_hash(block_data);

        return hashes[block_index];
    }

private:
    void open_file() const
    {
        if (!file_opened)
        {
            file_stream.open(path.string(), std::ios::binary);
            if (!file_stream)
            {
                throw std::runtime_error("Failed to open file: " + path.string());
            }
            file_opened = true;
        }
    }

    void close_file() const
    {
        if (file_opened)
        {
            file_stream.close();
            file_opened = false;
        }
    }

    boost::filesystem::path path;
    uintmax_t size;
    mutable bool file_opened{false};
    mutable std::ifstream file_stream;
    mutable vector<string> hashes;
};


class FileScanner {
public:
    FileScanner(
        const vector<string>& scan_dirs,
        const vector<string>& exclude_dirs,
        const bool            scan_level,
        const int             min_file_size,
        const vector<string>& file_masks,
        int const             block_size,
        const HashAlgorithm&  hash_algo)
        :
        scan_dirs_(scan_dirs),
        exclude_dirs_(exclude_dirs),
        scan_level_(scan_level),
        min_file_size_(min_file_size),
        file_masks_(file_masks),
        block_size_(block_size),
        hash_algo_(hash_algo)
    {

        for (const auto& mask : file_masks_)
        {
            string regex_str = mask;

            string result;
            for (char c : regex_str) {
                if (c == '.') {
                    result += "\\.";
                } else if (c == '*') {
                    result += ".*";
                } else if (c == '?') {
                    result += ".";
                } else {
                    result += c;
                }
            }

            regex_str = "^" + regex_str + "$";

            mask_regexes_.emplace_back(regex_str, std::regex::icase);
        }
    }

    auto scan_directories() -> vector<FileInfo>
    {
        vector<FileInfo> files;

        for (const auto& dir_str : scan_dirs_)
        {
            boost::filesystem::path dir(dir_str);
            if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir))
            {
                std::cerr << "Warning: Directory does not exist or is not a directory: "
                          << dir << std::endl;
                continue;
            }

            if (is_excluded(dir))
            {
                continue;
            }

            scan_directory(dir, files, 0);
        }

        return files;
    }

private:
    auto matches_masks(const boost::filesystem::path& file_path) const -> bool
    {
        if (file_masks_.empty())
        {
            return true;
        }

        string filename = file_path.filename().string();

        return std::ranges::any_of(mask_regexes_, [&filename](const std::regex& regex)
        {
            return std::regex_match(filename, regex);
        });
    }

    auto is_excluded(const boost::filesystem::path& dir) const -> bool
    {
        return std::ranges::any_of(exclude_dirs_, [&dir](const string& exclude_dir_str)
        {
            boost::filesystem::path exclude_dir(exclude_dir_str);

            return dir == exclude_dir
                || (  boost::filesystem::exists(exclude_dir)
                   && dir.string().find(canonical(exclude_dir).string()) == 0);
        });
    }

    void scan_directory(
        const boost::filesystem::path& dir,
        vector<FileInfo>&              files,
        int                            level)
    {
        if (!scan_level_ && level > 0)
        {
            return;
        }

        try
        {
            for (const auto& entry : boost::filesystem::directory_iterator(dir))
            {
                const boost::filesystem::path& path = entry.path();

                if (boost::filesystem::is_directory(path))
                {
                    if (!is_excluded(path))
                    {
                        scan_directory(path, files, level + 1);
                    }
                }
                else if (boost::filesystem::is_regular_file(path))
                {
                    uintmax_t file_size = boost::filesystem::file_size(path);
                    if (file_size >= min_file_size_)
                    {
                        if (matches_masks(path))
                        {
                            files.emplace_back(path, file_size, block_size_);
                        }
                    }
                }
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            std::cerr << "Error scanning directory " << dir << ": " << e.what() <<
                std::endl;
        }
    }

    vector<string> scan_dirs_;
    vector<string> exclude_dirs_;
    bool scan_level_;
    int min_file_size_;
    vector<string> file_masks_;
    vector<std::regex> mask_regexes_;
    int block_size_;
    HashAlgorithm hash_algo_;
};

class DuplicateFinder
{
public:
    DuplicateFinder(
        const int            block_size,
        const HashAlgorithm& hash_algo)
        :
        block_size_(block_size),
        hash_algo_(hash_algo) {}

    auto find_duplicates(vector<FileInfo>& files) -> vector<vector<FileInfo>>
    {
        unordered_map<uintmax_t, vector<FileInfo*>> files_by_size;

        for (auto& file : files)
        {
            files_by_size[file.get_size()].emplace_back(&file);
        }

        vector<vector<FileInfo>> duplicate_groups;

        for (auto& [size, size_group] : files_by_size)
        {
            if (size_group.size() < 2)
            {
                continue;
            }

            auto groups = find_duplicates_in_group(size_group);
            duplicate_groups.insert(duplicate_groups.end(), groups.begin(), groups.end());
        }

        return duplicate_groups;
    }

private:
    auto find_duplicates_in_group(vector<FileInfo*>& files) -> vector<vector<FileInfo>>
    {
        size_t num_blocks = (files[0]->get_size() + block_size_ - 1) / block_size_;

        for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx)
        {
            unordered_map<string, vector<FileInfo*>> files_by_hash;

            for (auto *file : files)
            {
                string hash =
                    file->compute_block_hash(block_idx, block_size_, hash_algo_);
                files_by_hash[hash].emplace_back(file);
            }

            files.clear();
            for (auto& [hash, hash_group] : files_by_hash)
            {
                if (hash_group.size() > 1)
                {
                    files.insert(files.end(), hash_group.begin(), hash_group.end());
                }
            }

            if (files.empty())
            {
                break;
            }
        }

        unordered_map<string, vector<FileInfo*>> files_by_all_hashes;
        for (auto *file : files)
        {
            string all_hashes;

            for (const auto& hash : file->get_hashes())
            {
                all_hashes += hash;
            }

            files_by_all_hashes[all_hashes].emplace_back(file);
        }

        vector<vector<FileInfo>> duplicate_groups;
        for (auto& [all_hashes, hash_group] : files_by_all_hashes)
        {
            if (hash_group.size() > 1)
            {
                vector<FileInfo> group;

                for (auto *file : hash_group)
                {
                    group.emplace_back(*file);
                }

                duplicate_groups.emplace_back(group);
            }
        }

        return duplicate_groups;
    }

    int block_size_;
    HashAlgorithm hash_algo_;
};

auto option_process(
    const int    argc,
    char **const argv) -> int
{
    bool scan_level = false;
    boost::program_options::options_description mandatory_options("Mandatory options");
    boost::program_options::options_description optional_options("Optional options");
    boost::program_options::options_description cmdline_options("All available options");
    boost::program_options::variables_map vm;
    HashAlgorithm hash_algorithm;
    int block_size = 0;
    int min_file_size = 0;
    int ret = 0;
    vector<string> exclude_dirs;
    vector<string> file_masks;
    vector<string> scan_dirs;

    auto negative_check = [](auto value)
    {
        if (value < 0)
        {
            throw boost::program_options::error("value must be non-negative, but got "
                + std::to_string(value));
        }
    };

    try
    {
        cmdline_options.add_options()
            ("help", "produce help message");
        mandatory_options.add_options()
            ("block_size", boost::program_options::value<decltype(block_size)>
                (&block_size)->required()->notifier(negative_check),
                "the block size used to read files (must be non-negative)")
            ("scan_dirs", boost::program_options::value<decltype(scan_dirs)>
                (&scan_dirs)->required(), "directories to scan (there may be several)")
            ("scan_level", boost::program_options::value<decltype(scan_level)>
                (&scan_level)->required(), "scanning level (one for all directories, "
                "0 - only the specified directory without nested ones)");

        optional_options.add_options()
            ("exclude_dirs", boost::program_options::value<decltype(exclude_dirs)>
                (&exclude_dirs),
                "directories to exclude from scanning (there may be several)")
            ("min_file_size", boost::program_options::value<decltype(min_file_size)>
                (&min_file_size)->default_value(1)->notifier(negative_check),
                "minimum file size, by default all files larger than 1 byte are checked.")
            ("file_masks", boost::program_options::value<decltype(file_masks)>
                (&file_masks),
                "masks of file names allowed for comparison (case-insensitive)")
            ("hash_algorithm", boost::program_options::value<string>()
                ->default_value("crc32")->notifier([&hash_algorithm](const string& value)
                {
                    try {
                        hash_algorithm = HashAlgorithm(value);
                    } catch (const boost::program_options::validation_error& e) {
                        throw e;
                    }
                }),
                "hashing algorithm to use (allowed values: crc32, md5)");

        cmdline_options.add(mandatory_options).add(optional_options);

        boost::program_options::store(boost::program_options::parse_command_line(argc,
            argv, cmdline_options), vm);

        if (vm.count("help") != 0U)
        {
            std::cout << cmdline_options;

            return ret;
        }

        boost::program_options::notify(vm);
    }
    catch (const boost::program_options::required_option & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << cmdline_options;
        ret = -1;
    }
    catch(const boost::program_options::error & e)
    {
        std::cout << e.what() << std::endl;
        ret = -2;
    }

    if (ret != 0) {
        std::cerr << "Error during parsing the arguments" << std::endl;

        return ret;
    }

    try
    {
        FileScanner scanner(scan_dirs, exclude_dirs, scan_level, min_file_size,
            file_masks, block_size, hash_algorithm);

        auto files = scanner.scan_directories();

        if (files.empty())
        {
            std::cout << "No files found matching the criteria." << std::endl;
            return ret;
        }

        DuplicateFinder finder(block_size, hash_algorithm);

        auto duplicate_groups = finder.find_duplicates(files);

        if (duplicate_groups.empty())
        {
            std::cout << "No duplicate files found." << std::endl;
        }
        else
        {
            for (const auto& group : duplicate_groups)
            {
                for (const auto& file : group)
                {
                    std::cout << file.get_path().string() << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        ret = -3;
    }

    return ret;
}

auto main(
    const int    argc,
    char **const argv) -> int
{
    int ret = option_process(argc, argv);

    if (ret != 0) {
        std::cerr << "option_process return " << ret << std::endl;
    }

    return ret;
}
