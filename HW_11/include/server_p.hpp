#ifndef SERVER_P_HPP
#define SERVER_P_HPP

#include <boost/asio.hpp>

#include <database.hpp>
#include <server.hpp>

class ServerImpl
{
    void do_accept();

    friend class Server;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
public:
    explicit ServerImpl(std::uint16_t port);
    explicit ServerImpl(
        std::promise<std::uint16_t>& port_promise,
        std::uint16_t                port);
    ~ServerImpl() = default;
    ServerImpl(const ServerImpl&) = delete;
    ServerImpl& operator=(const ServerImpl&) = delete;
    ServerImpl(ServerImpl&&) = delete;
    ServerImpl& operator=(ServerImpl&&) = delete;

    void run();
    void stop();
};

#endif // SERVER_P_HPP
