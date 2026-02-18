#include <charconv>
#include <iostream>
#include <string>
#include <string_view> // std::string_view
#include <system_error> // std::errc
#include <unordered_map>

namespace
{
    constexpr std::uint8_t PRICE_FIELD_INDEX = 9;

    struct Delimiter
    {
        char value;
    };

    struct FieldIndex
    {
        int value;
    };

    std::string extract_price_field(
        const std::string_view str_view,
        const Delimiter        delimiter,
        const FieldIndex       field_index)
    {
        bool in_quotes = false;
        int current_index{};
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
                    break;
                }

                current_field.clear();
                current_index++;
            }
            else if (current_index == field_index.value)
            {
                current_field += chr;
            }
        }

        return current_field;
    }
} // namespace

int main(int argc, char ** argv)
{
    std::unordered_map<double, int> price_counts;
    std::string line;
    while (std::getline(std::cin, line))
    {
        const std::string price_field =
            extract_price_field(line, {','}, {PRICE_FIELD_INDEX});
        double price = 0.0;

        if (price_field.empty())
        {
            continue;
        }

        const std::string_view price_field_sv(price_field);

        if (std::from_chars(price_field_sv.data(),
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            price_field_sv.data() + price_field_sv.size(), price,
            std::chars_format::general).ec == std::errc{})
        {
            price_counts[price]++;
        }
    }

    for (const auto& [price, count] : price_counts)
    {
        std::cout << "price\t" << price << "\t" << count << std::endl;
    }

    return 0;
}
