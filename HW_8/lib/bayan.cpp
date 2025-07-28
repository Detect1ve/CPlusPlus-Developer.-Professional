#include <iostream>

#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <bayan.hpp>

std::string compute_crc32(std::string_view input)
{
    boost::crc_32_type result;

    result.process_bytes(input.data(), input.length());

    return std::to_string(result.checksum());
}

std::string compute_md5(std::string_view input)
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;

    hash.process_bytes(input.data(), input.length());
    hash.get_digest(digest);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *const char_digest = reinterpret_cast<const char*>(&digest);
    std::string result;

    boost::algorithm::hex(char_digest, std::back_inserter(result));

    return result;
}

HashAlgorithm::HashAlgorithm() : value(crc32)
{
    hash_function = enum_to_func_map.at(value);
}

HashAlgorithm::HashAlgorithm(enum hash_algorithm value) : value(value)
{
    auto iterator = enum_to_func_map.find(value);

    if (iterator == enum_to_func_map.end())
    {
        throw std::invalid_argument(
            "Invalid hash algorithm enum value: " + std::to_string(value));
    }

    hash_function = iterator->second;
}

HashAlgorithm::HashAlgorithm(std::string_view name)
{
    std::string lower_s(name);
    std::ranges::transform(lower_s, lower_s.begin(), [](unsigned char character)
    {
        return static_cast<char>(std::tolower(character));
    });

    auto iterator = name_to_enum_map.find(lower_s);

    if (iterator == name_to_enum_map.cend())
    {
        throw boost::program_options::validation_error(
            boost::program_options::validation_error::invalid_option_value,
            "hash_algorithm", "Invalid hash algorithm");
    }

    value = iterator->second;
    hash_function = enum_to_func_map.at(value);
}

std::string HashAlgorithm::compute_hash(std::string_view input) const
{
    return hash_function(input);
}

FileInfo::FileInfo(
    boost::filesystem::path path,
    const uintmax_t         size,
    const std::size_t       block_size)
    :
    path(std::move(path)),
    hashes((size + block_size - 1) / block_size),
    size(size),
    block_size_(block_size) {}

FileInfo::FileInfo(const FileInfo& other)
    :
    path(other.path),
    hashes(other.hashes),
    size(other.size),
    block_size_(other.block_size_) {}

FileInfo& FileInfo::operator=(const FileInfo& other)
{
    if (this != &other)
    {
        close_file();

        path = other.path;
        hashes = other.hashes;
        size = other.size;
        block_size_ = other.block_size_;
        file_opened = false;
    }

    return *this;
}

FileInfo::~FileInfo()
{
    close_file();
}

uintmax_t FileInfo::get_size() const
{
    return size;
}

const std::vector<std::string>& FileInfo::get_hashes() const
{
    return hashes;
}

boost::filesystem::path FileInfo::get_path() const
{
    return path;
}

std::string FileInfo::compute_block_hash(
    const std::size_t    block_index,
    const HashAlgorithm& hash_algo) const
{
    if (!hashes[block_index].empty())
    {
        return hashes[block_index];
    }

    if (!file_opened)
    {
        open_file();
    }

    file_stream.seekg(static_cast<std::streamoff>(block_index * block_size_));

    std::vector<char> buffer(block_size_, 0);

    file_stream.read(buffer.data(), static_cast<std::streamsize>(block_size_));

    const std::streamsize bytes_read = file_stream.gcount();

    const std::string block_data(buffer.data(), bytes_read);

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
    const bool         scan_level,
    const BlockSize    block_size,
    const MinFileSize  min_file_size,
    const ExcludeDirs& exclude_dirs,
    const FileMasks&   file_masks,
    const ScanDirs&    scan_dirs)
    :
    scan_level_(scan_level),
    block_size_(block_size.value),
    min_file_size_(min_file_size.value),
    exclude_dirs_(exclude_dirs.value),
    file_masks_(file_masks.value),
    scan_dirs_(scan_dirs.value)
{
    for (const auto& mask : file_masks_)
    {
        const std::string regex_str = mask;

        std::string result;
        for (const char character : regex_str)
        {
            if (character == '.')
            {
                result += "\\.";
            }
            else if (character == '*')
            {
                result += ".*";
            }
            else if (character == '?')
            {
                result += ".";
            }
            else
            {
                result += character;
            }
        }

        const std::string final_regex = "^" + result + "$";

        mask_regexes_.emplace_back(final_regex, std::regex::icase);
    }
}

void FileScanner::scan_directory_recursive(
    const boost::filesystem::path& dir,
    std::vector<FileInfo>&         files)
{
    boost::filesystem::recursive_directory_iterator iterator(
        dir, boost::filesystem::directory_options::follow_directory_symlink);
    const boost::filesystem::recursive_directory_iterator end;

    while (iterator != end)
    {
        const boost::filesystem::path& path = iterator->path();

        if (is_excluded(path.parent_path()))
        {
            iterator.disable_recursion_pending();
            continue;
        }

        if (  boost::filesystem::is_regular_file(path)
           && boost::filesystem::file_size(path) >= min_file_size_
           && matches_masks(path))
        {
            files.emplace_back(path, boost::filesystem::file_size(path), block_size_);
        }

        ++iterator;
    }
}

void FileScanner::scan_directory_non_recursive(
    const boost::filesystem::path& dir,
    std::vector<FileInfo>&         files)
{
    for (const auto& entry : boost::filesystem::directory_iterator(dir))
    {
        const boost::filesystem::path& path = entry.path();
        if (  boost::filesystem::is_regular_file(path)
           && boost::filesystem::file_size(path) >= min_file_size_
           && matches_masks(path))
        {
            files.emplace_back(path, boost::filesystem::file_size(path), block_size_);
        }
    }
}

std::vector<FileInfo> FileScanner::scan_directories()
{
    std::vector<FileInfo> files;

    for (const auto& dir_str : scan_dirs_)
    {
        const boost::filesystem::path dir(dir_str);
        if (  !boost::filesystem::exists(dir)
           || !boost::filesystem::is_directory(dir))
        {
            std::cerr << "Warning: Directory does not exist or is not a directory: "
                << dir << '\n';
            continue;
        }

        if (is_excluded(dir))
        {
            continue;
        }

        try
        {
            if (scan_level_)
            {
                scan_directory_recursive(dir, files);
            }
            else
            {
                scan_directory_non_recursive(dir, files);
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            std::cerr << "Error scanning directory " << dir << ": " << e.what() << '\n';
        }
    }

    return files;
}

bool FileScanner::matches_masks(const boost::filesystem::path& file_path) const
{
    if (file_masks_.empty())
    {
        return true;
    }

    std::string filename = file_path.filename().string();

    return std::ranges::any_of(mask_regexes_, [&filename](const std::regex& regex)
    {
        return std::regex_match(filename, regex);
    });
}

bool FileScanner::is_excluded(const boost::filesystem::path& dir) const
{
    return std::ranges::any_of(exclude_dirs_, [&dir](const std::string& exclude_dir_str)
    {
        const boost::filesystem::path exclude_dir(exclude_dir_str);

        return dir == exclude_dir
            || (  boost::filesystem::exists(exclude_dir)
               && dir.string().starts_with(canonical(exclude_dir).string()));
    });
}


DuplicateFinder::DuplicateFinder(
    HashAlgorithm hash_algo,
    const int     block_size)
    :
    hash_algo_(std::move(hash_algo)),
    block_size_(block_size) {}

std::vector<std::vector<FileInfo>>
    DuplicateFinder::find_duplicates(std::vector<FileInfo>& files)
{
    std::unordered_map<uintmax_t, std::vector<FileInfo*>> files_by_size;

    for (auto& file : files)
    {
        files_by_size[file.get_size()].emplace_back(&file);
    }

    std::vector<std::vector<FileInfo>> duplicate_groups;

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

std::vector<std::vector<FileInfo>>
    DuplicateFinder::find_duplicates_in_group(std::vector<FileInfo*>& files)
{
    const std::size_t num_blocks = (files[0]->get_size() + block_size_ - 1) / block_size_;

    for (std::size_t block_idx = 0; block_idx < num_blocks; ++block_idx)
    {
        std::unordered_map<std::string, std::vector<FileInfo*>> files_by_hash;

        for (auto *file : files)
        {
            const std::string hash = file->compute_block_hash(block_idx, hash_algo_);

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

    std::unordered_map<std::string, std::vector<FileInfo*>> files_by_all_hashes;
    for (auto *file : files)
    {
        std::string all_hashes;

        for (const auto& hash : file->get_hashes())
        {
            all_hashes += hash;
        }

        files_by_all_hashes[all_hashes].emplace_back(file);
    }

    std::vector<std::vector<FileInfo>> duplicate_groups;
    for (auto& [all_hashes, hash_group] : files_by_all_hashes)
    {
        if (hash_group.size() > 1)
        {
            std::vector<FileInfo> group;

            for (auto *file : hash_group)
            {
                group.emplace_back(*file);
            }

            duplicate_groups.emplace_back(group);
        }
    }

    return duplicate_groups;
}

std::pair<ProcessStatus, Options> option_process(std::span<const char *const> argv)
{
    Options options;
    auto ret = ProcessStatus::SUCCESS;

    boost::program_options::options_description mandatory_options("Mandatory options");
    boost::program_options::options_description optional_options("Optional options");
    boost::program_options::options_description cmdline_options("All available options");
    boost::program_options::variables_map variables_map;

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
            ("hash_algorithm", boost::program_options::value<std::string>()
                ->default_value("crc32")->notifier([&options](const std::string& value)
                {
                    try
                    {
                        options.hash_algorithm = HashAlgorithm(value);
                    }
                    catch (const boost::program_options::validation_error&)
                    {
                        throw;
                    }
                }),
                "hashing algorithm to use (allowed values: crc32, md5)");

        cmdline_options.add(mandatory_options).add(optional_options);

        boost::program_options::store(boost::program_options::parse_command_line(
            static_cast<int>(argv.size()), argv.data(), cmdline_options), variables_map);

        if (variables_map.contains("help"))
        {
            std::cout << cmdline_options;

            return {ProcessStatus::HELP_REQUESTED, options};
        }

        boost::program_options::notify(variables_map);
    }
    catch (const boost::program_options::error& e)
    {
        std::cerr << e.what() << '\n';
        std::cerr << cmdline_options;
        ret = ProcessStatus::OPTION_ERROR;
    }

    return {ret, options};
}

ProcessStatus process_files(const Options& options)
{
    auto ret = ProcessStatus::SUCCESS;

    try
    {
        FileScanner scanner(options.scan_level, {options.block_size},
            {options.min_file_size}, {options.exclude_dirs}, {options.file_masks},
            {options.scan_dirs});

        auto files = scanner.scan_directories();

        if (files.empty())
        {
            std::cout << "No files found matching the criteria.\n";

            return ret;
        }

        DuplicateFinder finder(options.hash_algorithm, options.block_size);

        auto duplicate_groups = finder.find_duplicates(files);

        if (duplicate_groups.empty())
        {
            std::cout << "No duplicate files found.\n";
        }
        else
        {
            for (const auto& group : duplicate_groups)
            {
                for (const auto& file : group)
                {
                    std::cout << file.get_path().string() << '\n';
                }

                std::cout << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        ret = ProcessStatus::FILE_ERROR;
    }

    return ret;
}
