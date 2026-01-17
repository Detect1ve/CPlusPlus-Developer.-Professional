#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <mutex>
#include <unordered_map>
#include <vector>

struct Record
{
    int id;
    std::string name;
};

class Database
{
    std::mutex table_a_mutex_;
    std::mutex table_b_mutex_;
    std::unordered_map<int, std::string> table_a_;
    std::unordered_map<int, std::string> table_b_;
public:
    Database() = default;
    ~Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    bool insert(
        std::string_view table,
        int              record_id,
        std::string_view name);

    std::vector<std::string> intersection();
    std::vector<std::string> symmetric_difference();
    void truncate(std::string_view table);
};

#endif // DATABASE_HPP
