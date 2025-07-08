#pragma once
#if __GNUC__ < 14
#include <array>
#include <charconv>
#include <format>
#include <limits>
#include <span>
#endif
#include <expected>
#include <ranges>
#include <vector>

using std::error_code;
using std::expected;
using std::string;
using std::vector;

[[nodiscard]]
auto from_chars(std::span<const char> chars) -> expected<unsigned char, error_code>;

[[nodiscard]] constexpr auto split(
    const string& str,
    char          d) -> vector<string>;

void reverse_lexicographic_sort(vector<vector<string>>& ip_pool);

void print(std::span<const vector<string>> ip_pool);

template <typename First, typename... Rest>
    requires std::ranges::range<First> && (std::integral<Rest> && ...)
[[nodiscard]] auto filter(
    const First&   ip_pool,
    const Rest&... octet) -> expected<vector<vector<string>>, error_code>
{
    for (const auto& val : std::initializer_list<int>{static_cast<int>(octet)...})
    {
        if (  val < std::numeric_limits<unsigned char>::min()
           || val > std::numeric_limits<unsigned char>::max())
        {
            return std::unexpected(std::make_error_code(std::errc::value_too_large));
        }
    }

    std::array<int, sizeof...(octet)> a_octet = {octet...};

    return ip_pool | std::ranges::views::filter([&a_octet](const auto& ip)
    {
        auto common_range = std::min(a_octet.size(), ip.size());
        auto ip_view = ip | std::views::take(common_range);

        for (const auto& [idx, ip_part] : ip_view | std::views::enumerate)
        {
            auto result = from_chars(std::span<const char>(ip_part));
            if (!result)
            {
                return false;
            }

            if (result.value() != a_octet[idx])
            {
                return false;
            }
        }

        return true;
    }) | std::ranges::to<vector<vector<string>>>();
}

[[nodiscard]] auto filter_any(
    const vector<vector<string>>& ip_pool,
    unsigned char                 any_octet)
    -> expected<vector<vector<string>>, error_code>;

[[nodiscard]]
auto stdin_to_vector() -> expected<vector<vector<string>>, error_code>;
