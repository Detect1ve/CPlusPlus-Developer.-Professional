#include <gtest/gtest.h>

#include <my_list.hpp>

TEST(HW_3, my_list)
{
    MyList<int> my_list_with_standard_alloc;

    ASSERT_EQ(true, my_list_with_standard_alloc.empty());
    ASSERT_EQ(0, my_list_with_standard_alloc.size());

    my_list_with_standard_alloc.push_back(rand());

    ASSERT_EQ(false, my_list_with_standard_alloc.empty());
    ASSERT_EQ(1, my_list_with_standard_alloc.size());
}
