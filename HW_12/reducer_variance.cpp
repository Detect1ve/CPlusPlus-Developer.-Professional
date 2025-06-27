#include <cstdint>
#include <iostream>
#include <limits> // std::numeric_limits
#include <sstream>
#include <string> // std::string

int main()
{
    double sum_squared = 0.0;
    double sum = 0.0;
    std::string line;
    std::int64_t count = 0;

    while (std::getline(std::cin, line))
    {
        double price = std::numeric_limits<double>::quiet_NaN();
        double price_squared = std::numeric_limits<double>::quiet_NaN();
        std::istringstream iss(line);
        std::string key;
        std::uint16_t value_count = 0;

        if (iss >> key >> price >> price_squared >> value_count)
        {
            sum += price * static_cast<double>(value_count);
            sum_squared += price_squared * static_cast<double>(value_count);
            count += value_count;
        }
    }

    const double mean = (count > 0 ? sum / static_cast<double>(count) : 0.0);
    const double mean_squared =
        (count > 0 ? sum_squared / static_cast<double>(count) : 0.0);
    const double variance = mean_squared - (mean * mean);

    std::cout << "Variance: " << variance << '\n';

    return 0;
}
