#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

#include <sstream>

int main(int argc, char ** argv)
{
    size_t count = 0;
    std::string line;
    double sum = 0.0;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string key;
        double price = NAN;
        std::int64_t value_count = 0;

        if (iss >> key >> price >> value_count)
        {
            sum += price * static_cast<double>(value_count);
            count += value_count;
        }
    }

    const double mean = (count > 0 ? sum / static_cast<double>(count) : 0.0);

    std::cout << "Mean price: " << mean << std::endl;

    return 0;
}
