#include "lib.h"

#include <gtest/gtest.h>

TEST(HW1, GTestVersion) {
	EXPECT_GT(version(), 0);
}
