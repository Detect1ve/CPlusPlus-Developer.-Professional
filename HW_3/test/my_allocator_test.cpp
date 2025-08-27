#include <gtest/gtest.h>

#include <my_allocator.hpp>

constexpr int CONTAINER_SIZE = 10;

TEST(HW3, MyAllocator)
{
    std::vector<int, MyAllocator<int, CONTAINER_SIZE>> vector_with_custom_alloc;

    ASSERT_THROW(
    {
        vector_with_custom_alloc.reserve(CONTAINER_SIZE + 1);
    }, std::bad_alloc);
}
