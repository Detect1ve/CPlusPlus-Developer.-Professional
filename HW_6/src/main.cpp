#include <iostream>

#include <matrix.hpp>

enum
{
    CONTAINER_SIZE = 10,
};

auto main() -> int
{
    Matrix<int, 0> matrix;

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        matrix[i][i] = i;
    }

    for (int i = 0; i < CONTAINER_SIZE; i++)
    {
        matrix[CONTAINER_SIZE - i - 1][i] = CONTAINER_SIZE - i - 1;
    }

    for (int i = 1; i < CONTAINER_SIZE - 1; i++)
    {
        for (int j = 1; j < CONTAINER_SIZE - 1; j++)
        {
            std::cout << matrix[i][j] << " ";
        }

        std::cout << std::endl;
    }

    std::cout << matrix.size() << std::endl;

    for (auto elem : matrix)
    {
        auto [x, y, v] = elem;

        std::cout << x << " " << y << " " << v << std::endl;
    }

    return 0;
}
