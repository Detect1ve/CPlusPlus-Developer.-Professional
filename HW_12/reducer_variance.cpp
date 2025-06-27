#include <iostream>
#include <sstream>

auto main() -> int
{
    std::string line;
    double sum = 0.0;
    double sum_squared = 0.0;
    long count = 0;

    while (std::getline(std::cin, line))
    {
        double price;
        double price_squared;
        long value_count;
        std::istringstream iss(line);
        std::string key;

        if (iss >> key >> price >> price_squared >> value_count)
        {
            sum += price * value_count;
            sum_squared += price_squared * value_count;
            count += value_count;
        }
    }

    double mean = (count > 0) ? (sum / count) : 0.0;
    double mean_squared = (count > 0) ? (sum_squared / count) : 0.0;
    double variance = mean_squared - (mean * mean);

    std::cout << "Variance: " << variance << std::endl;

    return 0;
}
