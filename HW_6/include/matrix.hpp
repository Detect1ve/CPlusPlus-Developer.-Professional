#pragma once

#include <unordered_map>

using std::pair;
using std::unordered_map;

template <typename T, T DefaultValue>
class Matrix
{
    struct PairHash
    {
        auto operator()(const pair<int, int>& p) const noexcept -> std::size_t
        {
            return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
        }
    };

    unordered_map<pair<int, int>, T, PairHash> data;

    class ProxyRow
    {
        Matrix& matrix;
        int row;

    public:
        ProxyRow(
            Matrix&   m,
            int const r)
            :
            matrix(m),
            row(r) {}

        class ProxyCell
        {
            Matrix& matrix;
            int row;
            int col;

        public:
            ProxyCell(
                Matrix&   m,
                int const r,
                int const c)
                :
                matrix(m),
                row(r),
                col(c) {}

            auto operator=(const T& value) -> ProxyCell&
            {
                pair<int, int> key{row, col};

                if (value == DefaultValue)
                {
                    matrix.data.erase(key);
                }
                else
                {
                    matrix.data[key] = value;
                }

                return *this;
            }

            operator T() const
            {
                pair<int, int> key{row, col};
                auto it = matrix.data.find(key);

                if (it != matrix.data.end())
                {
                    return it->second;
                }

                return DefaultValue;
            }

            auto operator=(const ProxyCell& other) -> ProxyCell&
            {
                if (this != &other)
                {
                    T value = other;
                    *this = value;
                }

                return *this;
            }
        };

        auto operator[](int col) -> ProxyCell
        {
            return ProxyCell(matrix, row, col);
        }
    };

public:
    auto operator[](int row) -> ProxyRow
    {
        return ProxyRow(*this, row);
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return data.size();
    }

    class iterator
    {
        typename unordered_map<pair<int, int>, T, PairHash>::const_iterator it;

    public:
        iterator(typename unordered_map<pair<int, int>, T, PairHash>::const_iterator iter)
            : it(iter) {}

        [[nodiscard]] auto operator!=(const iterator& other) const -> bool
        {
            return it != other.it;
        }

        auto operator++() -> iterator&
        {
            ++it;

            return *this;
        }

        [[nodiscard]] auto operator*() const -> std::tuple<int, int, T>
        {
            return std::make_tuple(it->first.first, it->first.second, it->second);
        }
    };

    [[nodiscard]] auto begin() const noexcept -> iterator
    {
        return iterator(data.begin());
    }

    [[nodiscard]] auto end() const noexcept -> iterator
    {
        return iterator(data.end());
    }
};
