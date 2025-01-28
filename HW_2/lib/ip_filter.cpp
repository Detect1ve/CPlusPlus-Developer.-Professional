#include <iostream>

#include <include/ip_filter.hpp>
#include <vector>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]

auto split(
    const std::string & str,
    const char          d) -> std::vector<std::string>
{
    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    std::vector<std::string> r;

    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

auto comp(
    const std::vector<std::string>& a,
    const std::vector<std::string>& b) -> bool
{
    for (auto first = a.cbegin(), second = b.cbegin(); first != a.cend() && second != b.cend(); ++first, ++second)
    {
        unsigned char first_number = 0;
        unsigned char second_number = 0;

        std::from_chars(first->data(), first->data() + first->size(), first_number);
        std::from_chars(second->data(), second->data() + second->size(), second_number);

        if (first_number == second_number)
        {
            continue;
        }

        if (first_number > second_number)
        {
            return true;
        }

        return false;
    }

    return false;
}

auto reverse_lexicographic_sort(std::vector<std::vector<std::string>> & ip_pool) -> void
{
    std::sort(ip_pool.begin(), ip_pool.end(), comp);
}

auto filter_any(
    const std::vector<std::vector<std::string>>& ip_pool,
    const unsigned char                          any_octet) -> std::vector<std::vector<std::string>>
{
    std::vector<std::vector<std::string>> result;

    for (const auto & ip : ip_pool)
    {
        bool need_to_add = false;

        for (const auto & ip_part : ip)
        {
            unsigned char number = 0;

            std::from_chars(ip_part.data(), ip_part.data() + ip_part.size(), number);
            if (number == any_octet)
            {
                need_to_add = true;
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

auto print(const std::vector<std::vector<std::string>>& ip_pool) -> void
{
    for (const auto & ip : ip_pool)
    {
        for (auto ip_part = ip.cbegin(); ip_part != ip.cend(); ++ip_part)
        {
            if (ip_part != ip.cbegin())
            {
                std::cout << ".";
            }

            std::cout << *ip_part;
        }

        std::cout << std::endl;
    }
}


auto stdin_to_vector() -> std::vector<std::vector<std::string>>
{
    std::vector<std::vector<std::string>> result;

    try
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            result.push_back(split(v.at(0), '.'));
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return result;
}
