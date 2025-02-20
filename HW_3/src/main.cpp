#include <vector>
#include <iostream>

template <typename T> class MyAllocator {
public:
    typedef T value_type;
    MyAllocator() noexcept {}

    auto allocate(std::size_t n) -> T*
    {
        return static_cast<T*>(malloc(sizeof(T) * n));
    }

    void deallocate(T* p, std::size_t /* n */) noexcept
    {
        free(p);
    }
};

auto main() -> int
{
    std::vector<int, MyAllocator<int>> vec;

    for (int i = 0; i <= 5; i++)
    {
        vec.push_back(i);
    }

    for (const auto& elem : vec)
    {
        std::cout << elem << std::endl;
    }

    return 0;
}
