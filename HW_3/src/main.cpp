#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <functional> // std::less
#include <map>
#include <utility> // std::pair

#include <custom_print.hpp>
#include <my_allocator.hpp>
#include <my_list.hpp>

constexpr unsigned char CONTAINER_SIZE = 10;

namespace
{
    constexpr unsigned factorial(const unsigned number)
    {
        unsigned ret = 1;

        for (unsigned i = 2; i <= number; i++)
        {
            ret *= i;
        }

        return ret;
    }
} // namespace

int main()
{
    try
    {
        std::map<int, int> map_with_standard_alloc;

        for (unsigned i = 0; i < CONTAINER_SIZE; i++)
        {
            map_with_standard_alloc.insert({i, factorial(i)});
        }

        std::map<int, int, std::less<>,
            MyAllocator<std::pair<const int, int>, CONTAINER_SIZE>>
                map_with_custom_alloc;

        for (unsigned i = 0; i < CONTAINER_SIZE; i++)
        {
            map_with_custom_alloc.insert({i, factorial(i)});
        }

        for (const auto& elem : map_with_custom_alloc)
        {
            cp::println("{} {}", elem.first, elem.second);
        }

        MyList<unsigned> my_list_with_standard_alloc;

        for (unsigned i = 0; i < CONTAINER_SIZE; i++)
        {
            my_list_with_standard_alloc.push_back(i);
        }

        MyList<unsigned, MyAllocator<int, CONTAINER_SIZE>> my_list_with_custom_alloc;

        for (unsigned i = 0; i < CONTAINER_SIZE; i++)
        {
            my_list_with_custom_alloc.push_back(i);
        }

        for (const auto elem : my_list_with_custom_alloc)
        {
            cp::println("{}", elem);
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error(nullptr);

        return EXIT_FAILURE;
    }
}
