#include <charconv>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

auto extract_price_field(
    std::string_view s,
    const char       delimiter,
    const int        field_index) -> std::string
{
    bool in_quotes = false;
    int current_index = 0;
    std::string current_field;

    for (char c : s)
    {
        if (c == '"')
        {
            in_quotes = !in_quotes;
        }
        else if (c == delimiter && !in_quotes)
        {
            if (current_index == field_index)
            {
                return current_field;
            }

            current_field.clear();
            current_index++;
        }
        else if (current_index == field_index)
        {
            current_field += c;
        }
    }

    if (current_index == field_index)
    {
        return current_field;
    }

    return "";
}

auto main() -> int
{
    std::map<double, int> price_counts;
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::string price_field = extract_price_field(line, ',', 9);

        if (!price_field.empty())
        {
            double price = 0.0;
            auto [ptr, ec] = std::from_chars(price_field.data(),
                price_field.data() + price_field.size(), price);
            if (ec == std::errc())
            {
                price_counts[price]++;
            }
        }
    }

    for (const auto& [price, count] : price_counts)
    {
        double price_squared = price * price;

        std::cout << "price\t" << price << "\t" << price_squared << "\t" << count
            << std::endl;
    }

    return 0;
}
