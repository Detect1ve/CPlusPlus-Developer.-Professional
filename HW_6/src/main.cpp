#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception

#include <custom_print.hpp>
#include <matrix.hpp>

constexpr int CONTAINER_SIZE = 10;

int main()
{
    try
    {
        // todo unsigned
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
                cp::print("{} ", matrix[i][j]);
            }

            cp::println();
        }

        cp::println("{}", matrix.size());

        cp::print("{}", matrix);
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error();

        return EXIT_FAILURE;
    }

    return 0;
}
