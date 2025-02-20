#pragma once

#include <cstdlib>

template <typename T, const std::size_t size>
class MyAllocator
{
public:
    typedef T value_type;

    auto allocate(const std::size_t n) -> T*
    {
        if (memory_alloc + n > size)
        {
            throw std::bad_alloc();
        }

        memory_alloc += n;

        return static_cast<T*>(malloc(sizeof(T) * n));
    }

    template <typename U>
    struct rebind
    {
        using other = MyAllocator<U, size>;
    };

    void deallocate(
        T* const          p,
        const std::size_t n)
    {
        memory_alloc -= n;
        free(p);
    }

private:
    std::size_t memory_alloc = 0;
};
