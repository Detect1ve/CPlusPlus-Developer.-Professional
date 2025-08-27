#ifndef SERVER_HPP
#define SERVER_HPP

#include <future>
#include <memory>

class ServerImpl;
class Database;

class Server
{
    std::unique_ptr<ServerImpl> pimpl_;
public:
    explicit Server(std::int16_t port);
    explicit Server(
        std::promise<std::uint16_t>& port_promise,
        std::int16_t                 port);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void run();
    void setup_signal_handling();
    void stop();
};

#endif // SERVER_HPP
