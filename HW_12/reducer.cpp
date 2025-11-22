#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char ** argv)
{
    size_t count = 0;
    std::string line;
    float sum = 0.0;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string key;
        float price = NAN;
        std::size_t value_count = 0;

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
