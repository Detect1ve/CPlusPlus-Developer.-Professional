#include <algorithm>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)\
 || defined(__clang__)\
 || __cplusplus <=  202002L\
 || __GNUC__ < 14
#include <charconv>
#endif
#include <format> // std::format
#include <mutex> // std::scoped_lock
#include <set>
#include <sstream>
#include <string> // std::string
#include <string_view> // std::string_view
#include <system_error> // std::errc
#include <vector> // std::vector

#include <database.hpp>

bool Database::insert(
    const std::string_view table,
    const int              record_id,
    const std::string_view name)
{
    if (table == "A")
    {
        const std::scoped_lock<std::mutex> lock(table_a_mutex_);
        if (table_a_.contains(record_id))
        {
            return false;
        }

        table_a_[record_id] = name;

        return true;
    }

    if (table == "B")
    {
        const std::scoped_lock<std::mutex> lock(table_b_mutex_);

        if (table_b_.contains(record_id))
        {
            return false;
        }

        table_b_[record_id] = name;

        return true;
    }

    return false;
}

void Database::truncate(const std::string_view table)
{
    if (table == "A")
    {
        const std::scoped_lock<std::mutex> lock(table_a_mutex_);

        table_a_.clear();
    }
    else if (table == "B")
    {
        const std::scoped_lock<std::mutex> lock(table_b_mutex_);

        table_b_.clear();
    }
}

std::vector<std::string> Database::intersection()
{
    std::set<int> common_ids;
    std::vector<std::string> result;

    const std::scoped_lock<std::mutex, std::mutex> lock(table_a_mutex_, table_b_mutex_);

    for (const auto& [record_id, unused_value] : table_a_)
    {
        if (table_b_.contains(record_id))
        {
            common_ids.insert(record_id);
        }
    }

    result.reserve(common_ids.size());

    for (const int record_id : common_ids)
    {
        result.emplace_back(std::format("{},{},{}", record_id, table_a_[record_id],
            table_b_[record_id]));
    }

    return result;
}

std::vector<std::string> Database::symmetric_difference()
{
    std::set<int> all_ids;
    std::set<int> common_ids;
    std::vector<std::string> result;

    const std::scoped_lock<std::mutex, std::mutex> lock(table_a_mutex_, table_b_mutex_);

    for (const auto& [record_id, unused_value] : table_a_)
    {
        all_ids.insert(record_id);
        if (table_b_.contains(record_id))
        {
            common_ids.insert(record_id);
        }
    }

    for (const auto& [record_id, unused_value] : table_b_)
    {
        all_ids.insert(record_id);
    }

    for (const int record_id : all_ids)
    {
        if (!common_ids.contains(record_id))
        {
            std::ostringstream oss;
            oss << record_id << ",";

            if (table_a_.contains(record_id))
            {
                oss << table_a_[record_id];
            }

            oss << ",";

            if (table_b_.contains(record_id))
            {
                oss << table_b_[record_id];
            }

            result.emplace_back(oss.str());
        }
    }

    std::ranges::sort(result, [](
        const std::string_view string_a,
        const std::string_view string_b)
    {
        auto substring_a = string_a.substr(0, string_a.find(','));
        auto substring_b = string_b.substr(0, string_b.find(','));
        constexpr unsigned char BASE = 10;
        int id_a = 0;
        int id_b = 0;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (std::from_chars(substring_a.data(), substring_a.data() + substring_a.size(),
            id_a, BASE).ec != std::errc{})
        {
            return false;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (std::from_chars(substring_b.data(), substring_b.data() + substring_b.size(),
            id_b, BASE).ec != std::errc{})
        {
            return false;
        }

        return id_a < id_b;
    });

    return result;
}
