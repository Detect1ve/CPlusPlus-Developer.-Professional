#include <gtest/gtest.h>

#include <my_allocator.hpp>

enum
{
    CONTAINER_SIZE = 10,
};

TEST(HW_3, my_allocator)
{
    std::vector<int, MyAllocator<int, CONTAINER_SIZE>> vector_with_custom_alloc;

    try
    {
        vector_with_custom_alloc.reserve(CONTAINER_SIZE + 1);
        FAIL() << "std::bad_alloc";
    }
    catch(std::bad_alloc const & err)
    {
        ASSERT_EQ(err.what(), std::string("std::bad_alloc"));
    }
    catch(...)
    {
        FAIL() << "Expected std::bad_alloc";
    }
}
