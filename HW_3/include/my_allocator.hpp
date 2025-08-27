#ifndef MY_ALLOCATOR_HPP
#define MY_ALLOCATOR_HPP

#include <cstdlib>

#include <gsl/gsl>

template <typename T, const std::size_t size>
class MyAllocator
{
public:
    using value_type = T;

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
    MyAllocator() noexcept {}
#endif

    gsl::owner<T*> allocate(const std::size_t n)
    {
        if (memory_alloc + n > size)
        {
            throw std::bad_alloc();
        }

        memory_alloc += n;
        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,hicpp-no-malloc)
        return static_cast<T*>(malloc(sizeof(T) * n));
    }

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
    template <typename U>
    MyAllocator(const MyAllocator<U, size>&) noexcept {}
#endif

    template <typename U>
    struct rebind
    {
        using other = MyAllocator<U, size>;
    };

    void deallocate(
        gsl::owner<T*> const ptr,
        const std::size_t    n)
    {
        memory_alloc -= n;
        free(ptr); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
    }

private:
    std::size_t memory_alloc = 0;
};

#endif // MY_ALLOCATOR_HPP
