#include <algorithm>
#include <charconv> // std::from_chars
#include <cstddef> // std::size_t
#if !((defined(__clang_analyzer__) || defined(__clang__)) && (__clang_major__ <= 18))
#include <expected> // std::expected
#endif
#include <functional> // std::identity
#include <iostream>
#include <ranges> // std::ranges::views::take
#include <stdexcept> // std::out_of_range
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <string>
#endif
#include <span> // std::span
#include <string_view> // std::string_view
#include <system_error> // std::error_code
#include <vector> // std::vector

#include <custom_print.hpp>
#include <ip_filter.hpp>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]

std::expected<unsigned char, std::error_code> from_chars(const std::string_view chars)
{
    constexpr unsigned char BASE = 10;
    unsigned char value = 0;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (auto [ptr, ec] = std::from_chars(chars.data(), chars.data() + chars.size(),
        value, BASE); ec != std::errc{})
    {
        return std::unexpected(std::make_error_code(ec));
    }

    return value;
}

namespace
{
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
} // namespace

void reverse_lexicographic_sort(std::vector<std::vector<std::string>>& ip_pool)
{
    auto comp = [](
        const std::vector<std::string>& first_ip,
        const std::vector<std::string>& second_ip)
    {
        return std::ranges::lexicographical_compare(first_ip, second_ip, [](
            const std::string_view ip_1,
            const std::string_view ip_2)
        {
            return from_chars(ip_1).value_or(0) > from_chars(ip_2).value_or(0);
        });
    };

    std::ranges::sort(ip_pool, comp, std::identity{});
}

std::expected<std::vector<std::vector<std::string>>, std::error_code> filter_any(
    const std::vector<std::vector<std::string>>& ip_pool,
    const unsigned char                          any_octet)
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
    for (const auto& ip_address : ip_pool)
    {
#ifdef __cpp_lib_ranges_join_with
        auto ip_address_view = std::views::join_with(ip_address, '.');
#else
        std::vector<std::string_view> temp;

        temp.reserve(ip_address.size() * 2 - 1);

        for (std::size_t i = 0; i < ip_address.size(); i++)
        {
            temp.emplace_back(ip_address[i]);
            if (i != ip_address.size() - 1)
            {
                temp.emplace_back(".");
            }
        }

        auto ip_address_view = temp | std::views::join;
#endif
#ifdef __cpp_lib_format_ranges
        cp::println("{:s}", ip_address_view);
#else
        for (auto symbol : ip_address_view)
        {
            cp::print("{}", symbol);
        }

        cp::println();
#endif
    }
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
