#include <iostream>

#include <matrix.hpp>

constexpr char CONTAINER_SIZE = 10;

int main()
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

        std::cout << '\n';
    }

    std::cout << matrix.size() << '\n';

    for (auto elem : matrix)
    {
        auto [row, col, value] = elem;

        std::cout << row << " " << col << " " << value << '\n';
    }

    return 0;
}
