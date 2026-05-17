#include <new> // std::bad_alloc
#include <vector> // std::vector

#include <gtest/gtest.h>

#include <my_allocator.hpp>

constexpr unsigned char CONTAINER_SIZE = 10;

TEST(HW3, MyAllocator)
{
    ASSERT_THROW(([] {
        std::vector<int, MyAllocator<int, CONTAINER_SIZE>> vector_with_custom_alloc;

        vector_with_custom_alloc.reserve(CONTAINER_SIZE + 1);
    }()), std::bad_alloc);
}
