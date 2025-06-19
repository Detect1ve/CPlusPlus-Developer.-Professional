#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <format>
#include <unordered_map>

template <typename T, T DefaultValue>
class Matrix;

template <typename T, T DefaultValue>
class ProxyCell
{
public:
    ProxyCell(
        Matrix<T, DefaultValue>* matrix,
        const int                row, // NOLINT(bugprone-easily-swappable-parameters)
        const int                col)
        :
        col_(col),
        row_(row),
        matrix_(matrix) {}
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
    ProxyCell& operator=(const T& value)
    {
        if (value == DefaultValue)
        {
            matrix_->data_.erase({row_, col_});
        }
        else
        {
            matrix_->data_[{row_, col_}] = value;
        }

        return *this;
    }
    ProxyCell(ProxyCell&&) = default;
    ProxyCell& operator=(ProxyCell&&) = delete;

    operator T() const // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
        auto iter = matrix_->data_.find({row_, col_});

        return (iter != matrix_->data_.end() ? iter->second : DefaultValue);
    }

private:
    int col_;
    int row_;
    Matrix<T, DefaultValue>* matrix_;
};

template <typename T, T DefaultValue>
class Matrix
{
    class PairHash;
    class ProxyRow;
public:
    Matrix() : data_() {}

    ProxyRow operator[](int const row)
    {
        return ProxyRow(this, row);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return data_.size();
    }

    class iterator
    {
    public:
        explicit iterator(
            typename std::unordered_map<std::pair<int, int>, T, PairHash>::const_iterator
                iter)
            : it_(iter) {}

        [[nodiscard]] bool operator!=(const iterator& other) const
        {
            return it_ != other.it_;
        }

        iterator& operator++()
        {
            it_++;

            return *this;
        }

        [[nodiscard]] std::tuple<int, int, T> operator*() const
        {
            return std::make_tuple(it_->first.first, it_->first.second, it_->second);
        }

    private:
        typename std::unordered_map<std::pair<int, int>, T, PairHash>::const_iterator it_;
    };

    [[nodiscard]] iterator begin() const noexcept
    {
        return iterator(data_.begin());
    }

    [[nodiscard]] iterator end() const noexcept
    {
        return iterator(data_.end());
    }

private:
    class PairHash
    {
    public:
        std::size_t operator()(const std::pair<int, int>& coords) const noexcept
        {
            return
                std::hash<int>{}(coords.first) ^ (std::hash<int>{}(coords.second) << 1U);
        }
    };

    class ProxyRow
    {
    public:
        ProxyRow(
            Matrix *const matrix,
            int const     row)
            :
            row_(row),
            matrix_(matrix) {}

        ProxyCell<T, DefaultValue> operator[](int const col)
        {
            return ProxyCell<T, DefaultValue>(matrix_, row_, col);
        }

    private:
        int row_;
        Matrix* matrix_;
    };

    std::unordered_map<std::pair<int, int>, T, PairHash> data_;
    friend class ProxyCell<T, DefaultValue>;
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
