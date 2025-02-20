#include <algorithm>
#include <charconv>
#include <string>
#include <vector>

auto split(
    const std::string & str,
    char                d) -> std::vector<std::string>;

auto reverse_lexicographic_sort(std::vector<std::vector<std::string>> & ip_pool) -> void;

auto print(const std::vector<std::vector<std::string>>& ip_pool) -> void;

template <typename First, typename... Rest>
auto filter(
    const First &   ip_pool,
    const Rest &... octet) -> std::vector<std::vector<std::string>>
{
    std::array<int, sizeof...(octet)> a_octet = {octet...};
    std::vector<std::vector<std::string>> result;

    for (const auto & ip : ip_pool)
    {
        size_t i = 0;
        bool need_to_add = true;

        for (auto ip_part = ip.cbegin(); ip_part != ip.cend() && i != a_octet.size(); ++ip_part, ++i)
        {
            unsigned char number = 0;

            std::from_chars(ip_part->data(), ip_part->data() + ip_part->size(), number);
            if (number != a_octet[i])
            {
                need_to_add = false;
                break;
            }
        }

        if (need_to_add)
        {
            result.push_back(ip);
        }
    }

    return result;
}

auto filter_any(
    const std::vector<std::vector<std::string>>& ip_pool,
    unsigned char                                any_octet) -> std::vector<std::vector<std::string>>;


auto stdin_to_vector() -> std::vector<std::vector<std::string>>;
