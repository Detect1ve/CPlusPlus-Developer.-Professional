#include <algorithm>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER) \
 || defined(__clang__) \
 || __cplusplus <=  202002L
#include <charconv>
#endif
#include <set>
#include <sstream>

#include <database.h>

bool Database::insert(
    std::string_view table,
    const int        id,
    std::string_view name)
{
    if (table == "A")
    {
        const std::lock_guard<std::mutex> lock(table_a_mutex_);
        if (table_a_.contains(id))
        {
            return false;
        }

        table_a_[id] = name;

        return true;
    }

    if (table == "B")
    {
        const std::lock_guard<std::mutex> lock(table_b_mutex_);

        if (table_b_.contains(id))
        {
            return false;
        }

        table_b_[id] = name;

        return true;
    }

    return false;
}

void Database::truncate(std::string_view table)
{
    if (table == "A")
    {
        const std::lock_guard<std::mutex> lock(table_a_mutex_);

        table_a_.clear();
    }
    else if (table == "B")
    {
        const std::lock_guard<std::mutex> lock(table_b_mutex_);

        table_b_.clear();
    }
}

std::vector<std::string> Database::intersection()
{
    std::set<int> common_ids;
    std::vector<std::string> result;

    const std::lock_guard<std::mutex> lock_a(table_a_mutex_);
    const std::lock_guard<std::mutex> lock_b(table_b_mutex_);

    for (const auto& [id, _] : table_a_)
    {
        if (table_b_.contains(id))
        {
            common_ids.insert(id);
        }
    }

    for (const int id : common_ids)
    {
        std::ostringstream oss;

        oss << id << "," << table_a_[id] << "," << table_b_[id];
        result.emplace_back(oss.str());
    }

    return result;
}

std::vector<std::string> Database::symmetric_difference()
{
    std::set<int> all_ids;
    std::set<int> common_ids;
    std::vector<std::string> result;

    const std::lock_guard<std::mutex> lock_a(table_a_mutex_);
    const std::lock_guard<std::mutex> lock_b(table_b_mutex_);

    for (const auto& [id, _] : table_a_)
    {
        all_ids.insert(id);
        if (table_b_.contains(id))
        {
            common_ids.insert(id);
        }
    }

    for (const auto& [id, _] : table_b_)
    {
        all_ids.insert(id);
    }

    for (const int id : all_ids)
    {
        if (!common_ids.contains(id))
        {
            std::ostringstream oss;
            oss << id << ",";

            if (table_a_.contains(id))
            {
                oss << table_a_[id];
            }

            oss << ",";

            if (table_b_.contains(id))
            {
                oss << table_b_[id];
            }

            result.emplace_back(oss.str());
        }
    }

    std::ranges::sort(result, [](
        const std::string& a,
        const std::string& b)
    {
        auto substring_a = a.substr(0, a.find(','));
        auto substring_b = b.substr(0, b.find(','));
        int id_a = 0;
        int id_b = 0;

        {
            auto [ptr, ec] = std::from_chars(substring_a.data(),
                substring_a.data() + substring_a.size(), id_a);
            if (ec != std::errc())
            {
                return false;
            }
        }

        {
            auto [ptr, ec] = std::from_chars(substring_b.data(),
                substring_b.data() + substring_b.size(), id_b);
            if (ec != std::errc())
            {
                return false;
            }
        }

        return id_a < id_b;
    });

    return result;
}
