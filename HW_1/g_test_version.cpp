#include <gtest/gtest.h>
#include "lib.h"

TEST(HW_1, g_test_version) {
    EXPECT_GT(version(), 0);
}
