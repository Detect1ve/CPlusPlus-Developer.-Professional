#ifndef IP_FILTER_HPP
#define IP_FILTER_HPP

#if __GNUC__ < 14
#include <array>
#include <charconv>
#include <format>
#include <limits>
#include <span>
#endif
#if defined(__clang__) or __GNUC__ < 14
#include <utility>
#endif
#include <expected>
#include <ranges>
#include <vector>

[[nodiscard]]
std::expected<unsigned char, std::error_code> from_chars(std::span<const char> chars);

[[nodiscard]] constexpr std::vector<std::string> split(
    const std::string& str,
    char               d); // NOLINT(readability-identifier-length)

void reverse_lexicographic_sort(std::vector<std::vector<std::string>>& ip_pool);

void print(std::span<const std::vector<std::string>> ip_pool);

template <typename First, typename... Rest>
    requires std::ranges::range<First> && (std::integral<Rest> && ...)
[[nodiscard]]
std::expected<std::vector<std::vector<std::string>>, std::error_code> filter(
    const First&   ip_pool,
    const Rest&... octet)
{
    for (const auto& val : std::initializer_list<int>{static_cast<int>(octet)...})
    {
        if (  std::cmp_less(val, std::numeric_limits<unsigned char>::min())
           || std::cmp_greater(val, std::numeric_limits<unsigned char>::max()))
        {
            return std::unexpected(std::make_error_code(std::errc::value_too_large));
        }
    }

    std::array<int, sizeof...(octet)> a_octet = {octet...};

#if defined(__cpp_lib_ranges_to_container)
    return ip_pool | std::ranges::views::filter([&a_octet](const auto& ip)
    {
        auto common_range = std::min(a_octet.size(), ip.size());
        auto ip_view = ip | std::views::take(common_range);

        for (const auto& [idx, ip_part] : ip_view | std::views::enumerate)
        {
            auto result = from_chars(std::span<const char>(ip_part));
            if (!result || result.value() != a_octet[idx])
            {
                return false;
            }
        }

        return true;
    }) | std::ranges::to<std::vector<std::vector<std::string>>>();
#else
    std::vector<std::vector<std::string>> result;
    for (const auto& ip_address : ip_pool)
    {
        auto common_range = std::min(a_octet.size(), ip_address.size());
        bool match = true;

        for (size_t idx = 0; idx < common_range; ++idx)
        {
            auto res = from_chars(std::span<const char>(ip_address[idx]));
            if (  !res
               || res.value() != a_octet.at(idx))
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            result.emplace_back(ip_address);
        }
    }

    return result;
#endif
}

[[nodiscard]]
std::expected<std::vector<std::vector<std::string>>, std::error_code> filter_any(
    const std::vector<std::vector<std::string>>& ip_pool,
    unsigned char                                any_octet);

[[nodiscard]]
std::expected<std::vector<std::vector<std::string>>, std::error_code> stdin_to_vector();

#endif /* IP_FILTER_HPP */
