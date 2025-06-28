#include <iostream>

#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <bayan.hpp>

auto compute_crc32(string_view input) -> string
{
    boost::crc_32_type result;

    result.process_bytes(input.data(), input.length());

    return std::to_string(result.checksum());
}

auto compute_md5(string_view input) -> string
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

HashAlgorithm::HashAlgorithm() : value(crc32)
{
    hash_function = enum_to_func_map.at(value);
}

HashAlgorithm::HashAlgorithm(enum hash_algorithm v) : value(v)
{
    auto it = enum_to_func_map.find(value);

    if (it == enum_to_func_map.end())
    {
        throw std::invalid_argument(
            "Invalid hash algorithm enum value: " + std::to_string(value));
    }

    hash_function = it->second;
}

HashAlgorithm::HashAlgorithm(string_view s)
{
    string lower_s(s);
    std::transform(lower_s.begin(), lower_s.end(), lower_s.begin(), [](unsigned char c)
    {
        return std::tolower(c);
    });

    auto it = name_to_enum_map.find(lower_s);

    if (it == name_to_enum_map.cend())
    {
        throw boost::program_options::validation_error(
            boost::program_options::validation_error::invalid_option_value,
            "hash_algorithm", "Invalid hash algorithm");
    }

    value = it->second;
    hash_function = enum_to_func_map.at(value);
}

auto HashAlgorithm::compute_hash(string_view input) const -> string
{
    return hash_function(input);
}

FileInfo::FileInfo(
    const boost::filesystem::path& p,
    const uintmax_t                s,
    const size_t                   block_size)
    :
    path(p),
    hashes((s + block_size - 1) / block_size),
    block_size_(block_size),
    size(s) {}

FileInfo::FileInfo(const FileInfo& other)
    :
    path(other.path),
    file_opened(false),
    hashes(other.hashes),
    block_size_(other.block_size_),
    size(other.size) {}

auto FileInfo::operator=(const FileInfo& other) -> FileInfo&
{
    if (this != &other)
    {
        close_file();

        path = other.path;
        size = other.size;
        hashes = other.hashes;
        block_size_ = other.block_size_;
        file_opened = false;
    }

    return *this;
}

FileInfo::~FileInfo()
{
    close_file();
}

auto FileInfo::get_size() const -> uintmax_t
{
    return size;
}

auto FileInfo::get_hashes() const -> const vector<string>&
{
    return hashes;
}

auto FileInfo::get_path() const -> boost::filesystem::path
{
    return path;
}

auto FileInfo::compute_block_hash(
    const size_t         block_index,
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

    file_stream.seekg(block_index * block_size_);

    vector<char> buffer(block_size_, 0);

    file_stream.read(buffer.data(), block_size_);

    std::streamsize bytes_read = file_stream.gcount();

    string block_data(buffer.data(), bytes_read);

    hashes[block_index] = hash_algo.compute_hash(block_data);

    return hashes[block_index];
}

void FileInfo::open_file() const
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

void FileInfo::close_file() const
{
    if (file_opened)
    {
        file_stream.close();
        file_opened = false;
    }
}

FileScanner::FileScanner(
    const bool            scan_level,
    int const             block_size,
    const int             min_file_size,
    const vector<string>& exclude_dirs,
    const vector<string>& file_masks,
    const vector<string>& scan_dirs)
    :
    scan_level_(scan_level),
    block_size_(block_size),
    min_file_size_(min_file_size),
    exclude_dirs_(exclude_dirs),
    file_masks_(file_masks),
    scan_dirs_(scan_dirs)
{
    for (const auto& mask : file_masks_)
    {
        string regex_str = mask;

        string result;
        for (char c : regex_str)
        {
            if (c == '.')
            {
                result += "\\.";
            }
            else if (c == '*')
            {
                result += ".*";
            }
            else if (c == '?')
            {
                result += ".";
            }
            else
            {
                result += c;
            }
        }

        string final_regex = "^" + result + "$";

        mask_regexes_.emplace_back(final_regex, std::regex::icase);
    }
}

auto FileScanner::scan_directories() -> vector<FileInfo>
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

auto FileScanner::matches_masks(const boost::filesystem::path& file_path) const -> bool
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

auto FileScanner::is_excluded(const boost::filesystem::path& dir) const -> bool
{
    return std::ranges::any_of(exclude_dirs_, [&dir](const string& exclude_dir_str)
    {
        boost::filesystem::path exclude_dir(exclude_dir_str);

        return dir == exclude_dir
            || (  boost::filesystem::exists(exclude_dir)
               && dir.string().find(canonical(exclude_dir).string()) == 0);
    });
}

void FileScanner::scan_directory(
    const boost::filesystem::path& dir,
    vector<FileInfo>&              files,
    const int                      level)
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
        std::cerr << "Error scanning directory " << dir << ": " << e.what() << std::endl;

        throw;
    }
}

DuplicateFinder::DuplicateFinder(
    const HashAlgorithm& hash_algo,
    const int            block_size)
    :
    hash_algo_(hash_algo),
    block_size_(block_size) {}

auto DuplicateFinder::find_duplicates(vector<FileInfo>& files) -> vector<vector<FileInfo>>
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

auto DuplicateFinder::find_duplicates_in_group(vector<FileInfo*>& files)
    -> vector<vector<FileInfo>>
{
    const size_t num_blocks = (files[0]->get_size() + block_size_ - 1) / block_size_;

    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx)
    {
        unordered_map<string, vector<FileInfo*>> files_by_hash;

        for (auto *file : files)
        {
            string hash = file->compute_block_hash(block_idx, hash_algo_);

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

auto option_process(
    const int    argc,
    char **const argv) -> std::pair<int, Options>
{
    Options options;
    options.scan_level = false;
    options.block_size = 0;
    options.min_file_size = 0;
    int ret = 0;

    boost::program_options::options_description mandatory_options("Mandatory options");
    boost::program_options::options_description optional_options("Optional options");
    boost::program_options::options_description cmdline_options("All available options");
    boost::program_options::variables_map vm;

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
            ("help,h", "produce help message");
        mandatory_options.add_options()
            ("block_size", boost::program_options::value<decltype(options.block_size)>
                (&options.block_size)->required()->notifier(negative_check),
                "the block size used to read files (must be non-negative)")
            ("scan_dirs", boost::program_options::value<decltype(options.scan_dirs)>
                (&options.scan_dirs)->required(),
                "directories to scan (there may be several)")
            ("scan_level", boost::program_options::value<decltype(options.scan_level)>
                (&options.scan_level)->required(),
                "scanning level (one for all directories, "
                "0 - only the specified directory without nested ones)");

        optional_options.add_options()
            ("exclude_dirs",
                boost::program_options::value<decltype(options.exclude_dirs)>
                (&options.exclude_dirs),
                "directories to exclude from scanning (there may be several)")
            ("min_file_size",
                boost::program_options::value<decltype(options.min_file_size)>
                (&options.min_file_size)->default_value(1)->notifier(negative_check),
                "minimum file size, by default all files larger than 1 byte are checked.")
            ("file_masks", boost::program_options::value<decltype(options.file_masks)>
                (&options.file_masks),
                "masks of file names allowed for comparison (case-insensitive)")
            ("hash_algorithm", boost::program_options::value<string>()
                ->default_value("crc32")->notifier([&options](const string& value)
                {
                    try
                    {
                        options.hash_algorithm = HashAlgorithm(value);
                    }
                    catch (const boost::program_options::validation_error& e)
                    {
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

            return {ret, options};
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
        std::cout << cmdline_options;
        ret = -2;
    }

    if (ret != 0)
    {
        std::cerr << "Error during parsing the arguments" << std::endl;
    }

    return {ret, options};
}

auto process_files(const Options& options) -> int
{
    int ret = 0;

    try
    {
        FileScanner scanner(options.scan_level, options.block_size, options.min_file_size,
            options.exclude_dirs, options.file_masks, options.scan_dirs);

        auto files = scanner.scan_directories();

        if (files.empty())
        {
            std::cout << "No files found matching the criteria." << std::endl;

            return ret;
        }

        DuplicateFinder finder(options.hash_algorithm, options.block_size);

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
