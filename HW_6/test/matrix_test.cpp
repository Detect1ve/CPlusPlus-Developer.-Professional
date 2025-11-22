#include <gtest/gtest.h>

#include <capture.hpp>
#include <matrix.hpp>

constexpr std::uint16_t TEST_VALUE_1 = 314;
constexpr std::uint16_t TEST_VALUE_2 = 217;

TEST(HW6, Matrix)
{
    int value = 0;
    // infinite matrix int filled with values ​​-1
    Matrix<int, -1> matrix;
    std::string capturedStdout;

    ASSERT_EQ(0, matrix.size());

    auto a = matrix[0][0];

    ASSERT_EQ(-1, a);
    ASSERT_EQ(0, matrix.size());
    matrix[100][100] = TEST_VALUE_1;
    value = matrix[100][100];
    ASSERT_EQ(TEST_VALUE_1, value);
    ASSERT_EQ(1, matrix.size());

    // one row will be printed
    // 100100314
    StdoutCapture::Begin();
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

    capturedStdout = StdoutCapture::End();

    ASSERT_EQ("100100314\n", capturedStdout);

    ((matrix[100][100] = TEST_VALUE_1) = 0) = TEST_VALUE_2;
    value = matrix[100][100];

    ASSERT_EQ(TEST_VALUE_2, value);
    ASSERT_EQ(1, matrix.size());

    StdoutCapture::Begin();
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

    capturedStdout = StdoutCapture::End();
    ASSERT_EQ("100100217\n", capturedStdout);

}
