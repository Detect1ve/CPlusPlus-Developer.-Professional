#include <iostream>
#include <map>

#include <my_allocator.hpp>
#include <my_list.hpp>

enum
{
    CONTAINER_SIZE = 10,
};

constexpr auto factorial(const int number) -> int
{
    int ret = 1;

    for (int i = 2; i <= number; i++)
    {
        ret *= i;
    }

    return ret;
}

auto main() -> int
{
    std::map<int, int> map_with_standard_alloc;

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        map_with_standard_alloc.insert({i, factorial(i)});
    }

    std::map<int, int, std::less<int>,
        MyAllocator<std::pair<const int, int>, CONTAINER_SIZE>> map_with_custom_alloc;

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        map_with_custom_alloc.insert({i, factorial(i)});
    }

    for (const auto& elem : map_with_custom_alloc)
    {
        std::cout << elem.first << " " << elem.second << std::endl;
    }

    MyList<int> my_list_with_standard_alloc;

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        my_list_with_standard_alloc.push_back(i);
    }

    MyList<int, MyAllocator<int, CONTAINER_SIZE>> my_list_with_custom_alloc;

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        my_list_with_custom_alloc.push_back(i);
    }

    for (const auto& elem : my_list_with_custom_alloc)
    {
        std::cout << elem << std::endl;
    }

    return 0;
}
