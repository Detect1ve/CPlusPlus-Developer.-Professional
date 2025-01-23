#include <algorithm>
#include <charconv>
#include <iostream>

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

auto reverse_lexicographic_sort(std::vector<std::vector<std::string>> & ip_pool)
{
	std::sort(ip_pool.begin(), ip_pool.end(), comp);
}

template <typename First, typename... Rest>
auto filter(
    const First &   ip_pool,
    const Rest &... octet)
{
    std::array<int, sizeof...(octet)> a_octet = {octet...};
    std::vector<std::vector<std::string>> result;

    for (const auto & ip : ip_pool)
    {
        size_t i = 0;
        bool need_to_add = true;

        for (auto ip_part = ip.cbegin(); ip_part != ip.cend() && i != a_octet.size(); ++ip_part, ++i)
        {
            unsigned char number;

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
    const unsigned char any_octet)
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

auto print(const std::vector<std::vector<std::string>>& ip_pool)
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

    std::cout << std::endl;
}

auto main(int  /*argc*/, char const * /*argv*/[]) -> int
{
    try
    {
        std::vector<std::vector<std::string>> ip_pool;

        for (std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        // TODO reverse lexicographic sort
        reverse_lexicographic_sort(ip_pool);
        print(ip_pool);

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        auto ip = filter(ip_pool, 1);
        print(ip);

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        ip = filter(ip_pool, 46, 70);
        print(ip);

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        ip = filter_any(ip_pool, 46);
        print(ip);

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
