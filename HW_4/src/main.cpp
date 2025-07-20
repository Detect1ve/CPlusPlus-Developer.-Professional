/**
 * @file main.cpp
 * @brief Demonstration of different ways to print IP addresses using templates
 * @author Nikolay Grin
 * @date 2025
 *
 * @details The program demonstrates the use of C++ templates to print IP addresses of
 * various data types: integers, strings, containers, and tuples. For each data type, a
 * specialized version of the print_ip function is used.
 */

 #include <sfinae_ip_address.hpp>

/**
 * @brief The main function of the program
 *
 * @details Demonstrates the print_ip function for various data types:
 * - integer types of different bit depth
 * - strings
 * - containers (vector, list)
 * - tuples
 *
 * @return 0
 */
int main()
{
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    print_ip(int8_t {-1}); // 255
    print_ip(int16_t {0}); // 0.0
    print_ip(int32_t {2130706433}); // 127.0.0.1
    print_ip(int64_t {8875824491850138409});// 123.45.67.89.101.112.131.41
    print_ip(std::string{"Hello, World!"} ); // Hello, World!
    print_ip(std::vector<int>{100, 200, 300, 400}); // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip(std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    return 0;
}
