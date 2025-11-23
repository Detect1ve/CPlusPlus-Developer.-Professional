#include <algorithm>
#include <iostream>
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <string>
#endif

#include <ip_filter.hpp>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]

std::expected<unsigned char, std::error_code> from_chars(std::string_view chars)
{
    constexpr unsigned char BASE = 10;
    unsigned char value = 0;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    auto [ptr, ec] = std::from_chars(chars.data(), chars.data() + chars.size(), value,
        BASE);
    if (ec != std::errc())
    {
        return std::unexpected(std::make_error_code(ec));
    }

    return value;
}

std::vector<std::string> split(
    const std::string& str,
    const char         d) // NOLINT(readability-identifier-length)
{
    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    std::vector<std::string> r; // NOLINT(readability-identifier-length)

    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

namespace
{
    constexpr bool comp(
        const std::vector<std::string>& first_ip,
        const std::vector<std::string>& second_ip)
    {
        auto common_range = std::min(first_ip.size(), second_ip.size());
        auto first_view = first_ip | std::views::take(common_range);

        for (const auto [idx, ip_part] : first_view | std::views::enumerate)
        {
            auto first_result = from_chars(ip_part);
            auto second_result = from_chars(second_ip[static_cast<std::size_t>(idx)]);

            if (!first_result || !second_result)
            {
                continue;
            }

            if (auto result = first_result.value() <=> second_result.value();
                result != std::strong_ordering::equal)
            {
                return result == std::strong_ordering::greater;
            }
        }

        return false;
    }
} // namespace

void reverse_lexicographic_sort(std::vector<std::vector<std::string>>& ip_pool)
{
    std::ranges::sort(ip_pool, comp, std::identity{});
}

std::expected<std::vector<std::vector<std::string>>, std::error_code> filter_any(
    const std::vector<std::vector<std::string>>& ip_pool,
    const unsigned char                any_octet)
{
#ifdef __cpp_lib_ranges_to_container
    return ip_pool | std::ranges::views::filter([any_octet](const auto& ip_address)
    {
        return std::ranges::any_of(ip_address, [any_octet](const std::string_view ip_part)
        {
            auto result = from_chars(ip_part);
            if (!result)
            {
                return false;
            }

            return result.value() == any_octet;
        });
    }) | std::ranges::to<std::vector<std::vector<std::string>>>();
#else
    std::vector<std::vector<std::string>> result;

    for (const auto& ip_address : ip_pool)
    {
        bool found = false;

        for (const auto& ip_part : ip_address)
        {
            auto res = from_chars(ip_part);
            if (  res
               && res.value() == any_octet)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            result.emplace_back(ip_address);
        }
    }

    return result;
#endif
}

void print(std::span<const std::vector<std::string>> ip_pool)
{
#ifdef __cpp_lib_ranges_to_container
    for (const auto& ip_address : ip_pool)
    {
        std::cout << std::format("{}\n", ip_address
            | std::views::join_with(std::string_view("."))
            | std::ranges::to<std::string>());
    }
#else
    for (const auto& ip_address : ip_pool)
    {
        for (std::size_t i = 0; i < ip_address.size(); ++i)
        {
            std::cout << ip_address[i];
            if (i + 1 < ip_address.size())
            {
                std::cout << ".";
            }
        }

        std::cout << '\n';
    }
#endif
}

std::expected<std::vector<std::vector<std::string>>, std::error_code> stdin_to_vector()
{
    std::vector<std::vector<std::string>> ip_pool;

    for (std::string line; std::getline(std::cin, line);)
    {
        // NOLINTNEXTLINE(readability-identifier-length)
        std::vector<std::string> v = split(line, '\t');

        try
        {
            ip_pool.push_back(split(v.at(0), '.'));
        }
        catch (const std::out_of_range&)
        {
            return std::unexpected(std::make_error_code(std::errc::result_out_of_range));
        }
    }

    return ip_pool;
}
