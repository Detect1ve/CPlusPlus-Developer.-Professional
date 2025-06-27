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
        double price;
        long value_count;

        if (iss >> key >> price >> value_count)
        {
            sum += price * value_count;
            count += value_count;
        }
    }

    double mean = (count > 0) ? (sum / count) : 0.0;

    std::cout << "Mean price: " << mean << std::endl;

    return 0;
}
