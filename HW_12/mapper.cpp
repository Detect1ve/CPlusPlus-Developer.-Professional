#include <iostream>
#include <string>


int main(int argc, char ** argv)
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::cout << line << "\t" << "1" << std::endl;
    }

    return 0;
}