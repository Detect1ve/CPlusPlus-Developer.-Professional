#ifndef IP_FILTER_HPP
#define IP_FILTER_HPP

#if defined(__clang__)\
 || __GNUC__ < 14
#include <cstdint>
#endif
#if __GNUC__ < 14
#include <array>
#include <charconv>
#include <format>
#include <limits>
#include <span>
#endif
#include <ranges>
#include <utility>
#include <vector>
#if defined(__clang_analyzer__) && (__clang_major__ <= 18)
// Provide a dummy implementation of std::expected for older clang-tidy versions to allow
// parsing of function signatures and bodies without fatal errors.

namespace std
{
    template <class E>
    class unexpected // NOLINT(cert-dcl58-cpp)
    {
    public:
        explicit unexpected(const E& /*unused*/) {} // NOLINT(cert-dcl58-cpp)
    };

    template <class E>
    unexpected(E) -> unexpected<E>; // NOLINT(cert-dcl58-cpp)

    template <class T, class E>
    class expected // NOLINT(cert-dcl58-cpp)
    {
    public:
        expected() = default;
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        expected(const T& /*unused*/) {}
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        expected(const unexpected<E>& /*unused*/) {}

        explicit operator bool() const
        {
            return true;
        }

        T& value()
        {
            static T dummy_value;
            return dummy_value;
        }

        [[nodiscard]] E error() const
        {
            return {};
        }
    };
} // namespace std
#else
#include <expected>
#endif

[[nodiscard]]
std::expected<std::uint8_t, std::error_code> from_chars(std::string_view chars)
#ifndef _MSC_VER
    __attribute__((pure))
#endif
    ;

[[nodiscard]] std::vector<std::string> split(
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
    for (const auto& val : std::initializer_list<int>{octet...})
    {
        if (  std::cmp_less(val, std::numeric_limits<unsigned char>::min())
           || std::cmp_greater(val, std::numeric_limits<unsigned char>::max()))
        {
            return std::unexpected(std::make_error_code(std::errc::value_too_large));
        }
    }

    std::array<int, sizeof...(octet)> a_octet = {octet...};

#ifdef __cpp_lib_ranges_to_container
    return ip_pool | std::ranges::views::filter([&a_octet](const auto& ip_address)
    {
        auto common_range = std::min(a_octet.size(), ip_address.size());
        auto ip_view = ip_address | std::views::take(common_range);

        for (const auto& [idx, ip_part] : ip_view | std::views::enumerate)
        {
            auto result = from_chars(ip_part);
            if (!result || result.value() != a_octet.at(static_cast<std::size_t>(idx)))
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

        for (std::size_t idx = 0; idx < common_range; ++idx)
        {
            auto res = from_chars(ip_address[idx]);
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

#endif // IP_FILTER_HPP
