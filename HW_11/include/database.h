#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct Record
{
    int id;
    std::string name;
};

class Database
{
public:
    auto insert(
        std::string_view table,
        int              id,
        std::string_view name) -> bool;

    void truncate(std::string_view table);
    auto intersection() -> std::vector<std::string>;
    auto symmetric_difference() -> std::vector<std::string>;

private:
    std::mutex table_a_mutex_;
    std::mutex table_b_mutex_;
    std::unordered_map<int, std::string> table_a_;
    std::unordered_map<int, std::string> table_b_;
};
