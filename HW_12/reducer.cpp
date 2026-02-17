#include <cstddef> // size_t
#include <iostream>
#include <limits> // std::numeric_limits
#include <sstream>
#include <string>

int main(int argc, char ** argv)
{
    size_t count = 0;
    std::string line;
    float sum = 0.0;
    while (std::getline(std::cin, line))
    {
        float price = std::numeric_limits<float>::quiet_NaN();
        std::istringstream iss(line);
        std::size_t value_count = 0;
        std::string key;

        if (iss >> key >> price >> value_count)
        {
            sum += price * static_cast<float>(value_count);
            count += value_count;
        }
    }

    const float mean = (count > 0 ? sum / static_cast<float>(count) : 0.0F);

    std::cout << "Mean price: " << mean << std::endl;

    return 0;
}
