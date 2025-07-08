#include <algorithm>
#include <iostream>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <string>
#endif

#include <ip_filter.hpp>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]

auto from_chars(std::span<const char> chars) -> expected<unsigned char, error_code>
{
    unsigned char value = 0;
    auto [ptr, ec] = std::from_chars(chars.data(), chars.data() + chars.size(), value);

    if (ec != std::errc())
    {
        return std::unexpected(std::make_error_code(ec));
    }

    return value;
}

constexpr auto split(
    const string& str,
    const char    d) -> vector<string>
{
    string::size_type start = 0;
    string::size_type stop = str.find_first_of(d);
    vector<string> r;

    while (stop != string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

constexpr auto comp(
    const vector<string>& a,
    const vector<string>& b) -> bool
{
    auto common_range = std::min(a.size(), b.size());
    auto a_view = a | std::views::take(common_range);

    for (const auto& [idx, a_part] : a_view | std::views::enumerate)
    {
        auto first_result = from_chars(std::span<const char>(a_part));
        auto second_result = from_chars(std::span<const char>(b[idx]));

        if (!first_result || !second_result)
        {
            continue;
        }

        if (auto result = first_result.value() <=> second_result.value(); result != 0)
        {
            return result > 0;
        }
    }

    return false;
}

void reverse_lexicographic_sort(vector<vector<string>>& ip_pool)
{
    std::ranges::sort(ip_pool, comp);
}

auto filter_any(
    const vector<vector<string>>& ip_pool,
    const unsigned char           any_octet)
    -> expected<vector<vector<string>>, error_code>
{
    return ip_pool | std::ranges::views::filter([any_octet](const auto& ip)
    {
        return std::ranges::any_of(ip, [any_octet](const std::string_view ip_part)
        {
            auto result = from_chars(std::span<const char>(ip_part));
            if (!result) {
                return false;
            }

            return result.value() == any_octet;
        });
    }) | std::ranges::to<vector<vector<string>>>();
}

void print(std::span<const vector<string>> ip_pool)
{
    for (const auto& ip : ip_pool)
    {
        std::cout << std::format("{}\n", ip | std::views::join_with(std::string_view("."))
            | std::ranges::to<string>());
    }
}

auto stdin_to_vector() -> expected<vector<vector<string>>, error_code>
{
    vector<vector<string>> ip_pool;

    for (string line; std::getline(std::cin, line);)
    {
        vector<string> v = split(line, '\t');

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
