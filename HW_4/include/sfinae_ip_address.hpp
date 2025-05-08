/**
 * @file sfinae_ip_address.hpp
 * @brief Implementing functions for printing IP addresses using templates
 * @author Nikolay Grin
 * @date 2025
 *
 * @bug Doxygen has issues with trailing return type syntax (auto func() -> return_type).
 * The preferred code style would use trailing return type, but due to Doxygen
 * limitations, need to use the traditional syntax (return_type func()) instead. If
 * Doxygen is fixed in the future, the code should be refactored to use trailing return
 * type. The problem looks like: argument ip_address from the argument list of print_ip
 * has multiple '@param' documentation sections
 */

#ifndef SFINAE_IP_ADDRESS_HPP
#define SFINAE_IP_ADDRESS_HPP

#if defined(__clang__) || __GNUC__ < 14
#include <cstdint>
#endif
#if __GNUC__ < 14
#include <tuple>
#endif
#include <iostream>
#include <list>
#include <vector>
/**
 * @brief Output IP address for string type
 *
 * @details The definition of a string is done through an explicit comparison via
 * std::is_same_v<std::string, T>
 *
 * @tparam T Data type (must be std::string)
 *
 * @param[in] ip_address IP address as a string
 */
template <typename T>
void print_ip(T const& ip_address) requires(std::is_same_v<T, std::string>)
{
    std::cout << ip_address << std::endl;
}

/**
 * @brief Printing IP address for integer types
 *
 * @details Converts an integer value to IP address format, separating bytes with dots.
 * For example, int32_t(2130706433) will be output as "127.0.0.1".
 * The definition of the integral type is done via std::is_integral_v.
 *
 * @tparam T Integer data type
 *
 * @param[in] ip_address IP address as an integer
 */
template <typename T>
void print_ip(T const& ip_address) requires(std::is_integral_v<T>)
{
    constexpr std::size_t size = sizeof(T);

    for (std::size_t i = 0; i < size; ++i)
    {
        const auto byte = static_cast<std::uint8_t>(
            static_cast<std::make_unsigned_t<T>>(ip_address) >> ((size - 1 - i) * 8));

        std::cout << +byte;

        if (i < size - 1)
        {
            std::cout << '.';
        }
    }

    std::cout << '\n';
}

/**
 * @brief Basic template for checking that a type other than tuple will return false
 *
 * @tparam T Type of tuple
 */
 template <typename T>
 struct is_monotype_tuple : std::false_type {};

/**
 * @brief Specialization for std::tuple
 *
 * @details Checks that all types in a tuple match the type of the first element. For an
 * empty tuple or a tuple with one element, always returns true
 */
template <typename... Args>
struct is_monotype_tuple<std::tuple<Args...>> : std::bool_constant<(sizeof...(Args) <= 1)
    || (std::is_same_v<std::tuple_element_t<0, std::tuple<Args...>>, Args> && ...)> {};

/**
 * @brief Output IP address for tuples
 *
 * @details Prints an IP address represented as a tuple, where all elements have the same
 * type. For example, std::tuple(127, 0, 0, 1) will be printed as "127.0.0.1".
 *
 * @tparam T Type of tuple
 *
 * @param[in] ip_address IP address as a tuple
 */
template <typename T>
void print_ip(T const& ip_address) requires is_monotype_tuple<T>::value
{
    std::apply([](const auto&... args)
    {
        std::size_t idx = 0;

        ((std::cout << (idx++ == 0 ? "" : ".") << args), ...);
        std::cout << '\n';
    }, ip_address);
}

/**
 * @brief Basic template, by default type T is not a container
 *
 * @tparam T Type of container
 */
template <typename T, typename = void>
struct is_container : std::false_type {};

/**
 * @brief Specialization for containers
 *
 * @details Checks if a type has value_type and begin() method
 *
 * @tparam T Type of container
 */
template <typename T>
struct is_container<T, std::void_t<
    typename T::value_type, decltype(std::declval<T>().begin())>> : std::true_type {};

/**
 * @brief Output IP address for containers (vector, list, etc.)
 *
 * @details Prints an IP address as a container, separating elements with dots. For
 * example, std::vector<int>{127, 0, 0, 1} will be printed as "127.0.0.1".
 *
 * @tparam T Container type
 *
 * @param[in] ip_address IP address as a container
 */
template<typename T>
void print_ip(T const &ip_address)
    requires(is_container<T>::value && !std::is_same_v<T, std::string>)
{
    std::size_t size = ip_address.size();

    for (auto octet : ip_address)
    {
        std::cout << octet;
        if (--size)
        {
            std::cout << '.';
        }
    }

    std::cout << '\n';
}

#endif /* SFINAE_IP_ADDRESS_HPP */
