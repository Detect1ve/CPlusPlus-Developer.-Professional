#include <random>

#include <gtest/gtest.h>

#include <my_list.hpp>

TEST(HW3, MyList)
{
    std::random_device rand_device;
    std::default_random_engine gen(rand_device());
    std::uniform_int_distribution<> distrib;

    MyList<int> my_list_with_standard_alloc;

    ASSERT_EQ(true, my_list_with_standard_alloc.empty());
    ASSERT_EQ(0, my_list_with_standard_alloc.size());

    my_list_with_standard_alloc.push_back(distrib(gen));

    ASSERT_EQ(false, my_list_with_standard_alloc.empty());
    ASSERT_EQ(1, my_list_with_standard_alloc.size());
}
