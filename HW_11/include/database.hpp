#ifndef DATABASE_HPP
#define DATABASE_HPP

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
    bool insert(
        std::string_view table,
        int              record_id,
        std::string_view name);

    void truncate(std::string_view table);
    std::vector<std::string> intersection();
    std::vector<std::string> symmetric_difference();

private:
    std::mutex table_a_mutex_;
    std::mutex table_b_mutex_;
    std::unordered_map<int, std::string> table_a_;
    std::unordered_map<int, std::string> table_b_;
};

#endif /* DATABASE_HPP */
