#include <iostream>

#include <ip_filter.hpp>

int main(
    int          /*argc*/,
    char const * /*argv*/[])
{
    try
    {
        auto parse_result = stdin_to_vector();
        if (!parse_result)
        {
            std::cerr << "Cannot parse stdin: " << parse_result.error().message()
                << '\n';

            return -1;
        }

        // TODO reverse lexicographic sort
        reverse_lexicographic_sort(parse_result.value());

        print(parse_result.value());

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        auto ip_result = filter(parse_result.value(), 1);
        if (!ip_result)
        {
            std::cerr << std::format("Filter error: {}\n", ip_result.error().message());

            return -2;
        }

        print(ip_result.value());

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        ip_result = filter(parse_result.value(), 46, 70);
        if (!ip_result)
        {
            std::cerr << std::format("Filter error: {}\n", ip_result.error().message());

            return -3;
        }

        print(ip_result.value());

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        ip_result = filter_any(parse_result.value(), 46);
        if (!ip_result)
        {
            std::cerr << "Filter_any error\n";

            return -4;
        }
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

        print(ip_result.value());

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

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled exception: " << e.what() << '\n';

        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred\n";

        return EXIT_FAILURE;
    }
}
