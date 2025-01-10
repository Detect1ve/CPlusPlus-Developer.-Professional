#include "lib.h"

#include <gtest/gtest.h>

TEST(HW_1, g_test_version) {
	EXPECT_GT(version(), 0);
}
