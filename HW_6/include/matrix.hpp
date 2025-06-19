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
        Matrix* matrix_;
        int row_;

    public:
        ProxyRow(
            Matrix*   matrix,
            int const row)
            :
            matrix_(matrix),
            row_(row) {}

        class ProxyCell
        {
            Matrix* matrix_;
            int row_;
            int col_;

        public:
            ProxyCell(
                Matrix*   matrix,
                int const row, // NOLINT(bugprone-easily-swappable-parameters)
                int const col)
                :
                matrix_(matrix),
                row_(row),
                col_(col) {}

            ProxyCell(const ProxyCell&) = delete;
            ProxyCell(ProxyCell&&) = delete;
            ProxyCell& operator=(ProxyCell&&) = delete;
            ~ProxyCell() = default;

            ProxyCell& operator=(const T& value)
            {
                const std::pair<int, int> key{row_, col_};

                if (value == DefaultValue)
                {
                    matrix_->data.erase(key);
                }
                else
                {
                    matrix_->data[key] = value;
                }

                return *this;
            }

            // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
            operator T() const
            {
                const std::pair<int, int> key{row_, col_};
                auto iter = matrix_->data.find(key);

                if (iter != matrix_->data.end())
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
            return ProxyCell(matrix_, row_, col);
        }
    };

public:
    Matrix() : data() {}

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

#endif // MATRIX_HPP
