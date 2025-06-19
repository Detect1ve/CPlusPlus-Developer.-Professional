#pragma once

#include <algorithm>
#include <list>

template <typename T, T DefaultValue>
class Matrix
{
private:
    struct Item
    {
        Item(
            int const x_,
            int const y_,
            const T&  value_)
            :
            x(x_),
            y(y_),
            value(value_) {}

        [[nodiscard]] auto getX() const -> int
        {
            return x;
        }

        [[nodiscard]] auto getY() const -> int
        {
            return y;
        }

        [[nodiscard]] auto getValue() const -> T
        {
            return value;
        }

        void setValue(const T& newValue)
        {
            value = newValue;
        }

    private:
        int x;
        int y;
        T value;
    };

    std::list<Item> data;

    class ProxyRow
    {
    private:
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
        private:
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
                auto it = std::find_if(matrix.data.begin(), matrix.data.end(),
                    [this](const Item& item)
                    {
                        return item.getX() == row && item.getY() == col;
                    });

                if (value == DefaultValue) {
                    if (it != matrix.data.end())
                    {
                        matrix.data.erase(it);
                    }
                }
                else
                {
                    if (it != matrix.data.end())
                    {
                        it->setValue(value);
                    }
                    else
                    {
                        matrix.data.push_back(Item(row, col, value));
                    }
                }

                return *this;
            }

            operator T() const
            {
                auto it = std::find_if(matrix.data.begin(), matrix.data.end(),
                    [this](const Item& item)
                {
                    return item.getX() == row && item.getY() == col;
                });

                if (it != matrix.data.end())
                {
                    return it->getValue();
                }

                return DefaultValue;
            }

            auto operator=(const ProxyCell& other) -> ProxyCell&
            {
                T value = other;
                *this = value;
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

    [[nodiscard]] auto size() const -> size_t
    {
        return data.size();
    }

    class iterator
    {
    private:
        typename std::list<Item>::const_iterator it;

    public:
        iterator(typename std::list<Item>::const_iterator iter) : it(iter) {}

        auto operator!=(const iterator& other) const -> bool
        {
            return it != other.it;
        }

        auto operator++() -> iterator&
        {
            ++it;

            return *this;
        }

        auto operator*() const -> std::tuple<int, int, T>
        {
            return std::make_tuple(it->getX(), it->getY(), it->getValue());
        }
    };

    [[nodiscard]] auto begin() const -> iterator
    {
        return iterator(data.begin());
    }

    [[nodiscard]] auto end() const -> iterator
    {
        return iterator(data.end());
    }
};
