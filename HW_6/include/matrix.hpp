#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <format>
#include <unordered_map>

template <typename T, T DefaultValue>
class Matrix;

template <typename T, T DefaultValue>
class ProxyCell
{
    Matrix<T, DefaultValue>* matrix_;
    int row_;
    int col_;

public:
    ProxyCell(
        Matrix<T, DefaultValue>* matrix,
        const int                row, // NOLINT(bugprone-easily-swappable-parameters)
        const int                col)
        :
        matrix_(matrix),
        row_(row),
        col_(col) {}
    ~ProxyCell() = default;
    ProxyCell(const ProxyCell&) = delete;
    ProxyCell& operator=(const ProxyCell& other)
    {
        if (this != &other)
        {
            *this = static_cast<T>(other);
        }

        return *this;
    }
    ProxyCell(ProxyCell&&) = default;
    ProxyCell& operator=(ProxyCell&&) = delete;

    ProxyCell& operator=(const T& value)
    {
        if (value == DefaultValue)
        {
            matrix_->data.erase({row_, col_});
        }
        else
        {
            matrix_->data[{row_, col_}] = value;
        }

        return *this;
    }

    operator T() const // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
        auto iter = matrix_->data.find({row_, col_});

        return (iter != matrix_->data.end() ? iter->second : DefaultValue);
    }
};

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
    friend class ProxyCell<T, DefaultValue>;

    class ProxyRow
    {
        Matrix* matrix_;
        int row_;
    public:
        ProxyRow(
            Matrix *const matrix,
            int const     row)
            :
            matrix_(matrix),
            row_(row) {}

        ProxyCell<T, DefaultValue> operator[](const int col)
        {
            return ProxyCell<T, DefaultValue>(matrix_, row_, col);
        }
    };
public:
    Matrix() : data() {}

    ProxyRow operator[](const int row)
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
            it++;

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

template <typename T, T DefaultValue>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct std::formatter<ProxyCell<T, DefaultValue>> : std::formatter<T>
{
    auto format(
        const ProxyCell<T, DefaultValue>& cell,
        std::format_context&              ctx) const
    {
        return std::formatter<T>::format(static_cast<T>(cell), ctx);
    }
};

template <typename T, T DefaultValue>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct std::formatter<Matrix<T, DefaultValue>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(
        const Matrix<T, DefaultValue>& matrix,
        std::format_context&           ctx) const -> decltype(ctx.out())
    {
        auto out = ctx.out();

        for (auto const& [row, col, value] : matrix)
        {
            out = std::format_to(out, "{} {} {}\n", row, col, value);
        }

        return out;
    }
};

#endif // MATRIX_HPP
