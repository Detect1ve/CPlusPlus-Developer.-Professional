#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>

int main()
{
    float sum_squared = 0.0;
    float sum = 0.0;
    std::string line;
    std::int64_t count = 0;

    while (std::getline(std::cin, line))
    {
        float price = NAN;
        float price_squared = NAN;
        std::int64_t value_count = 0;
        std::istringstream iss(line);
        std::string key;

        if (iss >> key >> price >> price_squared >> value_count)
        {
            sum += price * static_cast<float>(value_count);
            sum_squared += price_squared * static_cast<float>(value_count);
            count += value_count;
        }
    }

    const float mean = (count > 0 ? sum / static_cast<float>(count) : 0.0F);
    const float mean_squared =
        (count > 0 ? sum_squared / static_cast<float>(count) : 0.0F);
    const float variance = mean_squared - (mean * mean);

    std::cout << "Variance: " << variance << '\n';

    return 0;
}
