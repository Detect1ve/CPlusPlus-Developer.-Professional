#include <gtest/gtest.h>

#include <matrix.hpp>

enum
{
    TEST_VALUE_1 = 314,
    TEST_VALUE_2 = 217,
};

TEST(HW_6, matrix)
{
    // infinite matrix int filled with values ​​-1
    Matrix<int, -1> matrix;
    std::string capturedStdout;

    ASSERT_EQ(0, matrix.size());

    auto a = matrix[0][0];

    ASSERT_EQ(-1, a);
    ASSERT_EQ(0, matrix.size());
    matrix[100][100] = TEST_VALUE_1;
    ASSERT_EQ(TEST_VALUE_1, matrix[100][100]);
    ASSERT_EQ(1, matrix.size());

    // one row will be printed
    // 100100314
    testing::internal::CaptureStdout();
    for (auto c: matrix)
    {
        int x = 0;
        int y = 0;
        int v = 0;

        std::tie(x, y, v) = c;

        std::cout << x << y << v << std::endl;

        ASSERT_EQ(100, x);
        ASSERT_EQ(100, y);
        ASSERT_EQ(TEST_VALUE_1, v);
    }

    capturedStdout = testing::internal::GetCapturedStdout();

    ASSERT_EQ("100100314\n", capturedStdout);

    ((matrix[100][100] = TEST_VALUE_1) = 0) = TEST_VALUE_2;

    ASSERT_EQ(TEST_VALUE_2, matrix[100][100]);
    ASSERT_EQ(1, matrix.size());

    testing::internal::CaptureStdout();
    for (auto c: matrix)
    {
        int x = 0;
        int y = 0;
        int v = 0;

        std::tie(x, y, v) = c;

        std::cout << x << y << v << std::endl;

        ASSERT_EQ(100, x);
        ASSERT_EQ(100, y);
        ASSERT_EQ(TEST_VALUE_2, v);
    }

    capturedStdout = testing::internal::GetCapturedStdout();
    ASSERT_EQ("100100217\n", capturedStdout);

}
