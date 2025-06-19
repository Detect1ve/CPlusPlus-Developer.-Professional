#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <unordered_map>

template <typename T, T DefaultValue>
class Matrix
{
    struct PairHash
    {
        std::size_t operator()(const std::pair<int, int>& coords) const noexcept
        {
            return
                std::hash<int>{}(coords.first) ^ (std::hash<int>{}(coords.second) << 1U);
        }
    };

    std::unordered_map<std::pair<int, int>, T, PairHash> data;

    class ProxyRow
    {
        Matrix* matrix;
        int row;

    public:
        ProxyRow(
            Matrix*   matrix,
            int const row)
            :
            matrix(matrix),
            row(row) {}

        class ProxyCell
        {
            Matrix* matrix;
            int row;
            int col;

        public:
            ProxyCell(
                Matrix*   matrix,
                int const row, // NOLINT(bugprone-easily-swappable-parameters)
                int const col)
                :
                matrix(matrix),
                row(row),
                col(col) {}

            ProxyCell(const ProxyCell&) = delete;
            ProxyCell(ProxyCell&&) = delete;
            ProxyCell& operator=(ProxyCell&&) = delete;
            ~ProxyCell() = default;

            ProxyCell& operator=(const T& value)
            {
                const std::pair<int, int> key{row, col};

                if (value == DefaultValue)
                {
                    matrix->data.erase(key);
                }
                else
                {
                    matrix->data[key] = value;
                }

                return *this;
            }

            // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
            operator T() const
            {
                const std::pair<int, int> key{row, col};
                auto iter = matrix->data.find(key);

                if (iter != matrix->data.end())
                {
                    return iter->second;
                }

                return DefaultValue;
            }

            ProxyCell& operator=(const ProxyCell& other)
            {
                if (this != &other)
                {
                    T value = other;
                    *this = value;
                }

                return *this;
            }
        };

        ProxyCell operator[](int col)
        {
            return ProxyCell(matrix, row, col);
        }
    };

public:
    ProxyRow operator[](int row)
    {
        return ProxyRow(this, row);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return data.size();
    }

    class iterator
    {
        typename std::unordered_map<std::pair<int, int>, T, PairHash>::const_iterator it;

    public:
        explicit iterator(
            typename std::unordered_map<std::pair<int, int>, T, PairHash>::const_iterator
                iter)
            : it(iter) {}

        [[nodiscard]] bool operator!=(const iterator& other) const
        {
            return it != other.it;
        }

        iterator& operator++()
        {
            ++it;

            return *this;
        }

        [[nodiscard]] std::tuple<int, int, T> operator*() const
        {
            return std::make_tuple(it->first.first, it->first.second, it->second);
        }
    };

    [[nodiscard]] iterator begin() const noexcept
    {
        return iterator(data.begin());
    }

    [[nodiscard]] iterator end() const noexcept
    {
        return iterator(data.end());
    }
};

#endif /* MATRIX_HPP */
