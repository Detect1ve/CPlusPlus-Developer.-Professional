#include <charconv>
#include <iostream>
#include <map>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <string>
#endif

namespace
{
    constexpr int PRICE_FIELD_INDEX = 9;

    struct Delimiter
    {
        char value;
    };

    struct FieldIndex
    {
        int value;
    };

    std::string extract_price_field(
        std::string_view str_view,
        const Delimiter  delimiter,
        const FieldIndex field_index)
    {
        bool in_quotes = false;
        int current_index = 0;
        std::string current_field;

        for (const char chr : str_view)
        {
            if (chr == '"')
            {
                in_quotes = !in_quotes;
            }
            else if (chr == delimiter.value && !in_quotes)
            {
                if (current_index == field_index.value)
                {
                    return current_field;
                }

                current_field.clear();
                current_index++;
            }
            else if (current_index == field_index.value)
            {
                current_field += chr;
            }
        }

        if (current_index == field_index.value)
        {
            return current_field;
        }

        return "";
    }
} // namespace

int main()
{
    std::map<double, int> price_counts;
    std::string line;

    while (std::getline(std::cin, line))
    {
        const std::string price_field =
            extract_price_field(line, {','}, {PRICE_FIELD_INDEX});

        if (!price_field.empty())
        {
            const std::string_view price_field_sv(price_field);
            double price = 0.0;
            auto [ptr, ec] = std::from_chars(price_field_sv.data(),
                price_field_sv.data() + price_field_sv.size(), price,
                std::chars_format::general);
            if (ec == std::errc())
            {
                price_counts[price]++;
            }
        }
    }

    for (const auto& [price, count] : price_counts)
    {
        const double price_squared = price * price;

        std::cout << "price\t" << price << "\t" << price_squared << "\t" << count << '\n';
    }

    return 0;
}
