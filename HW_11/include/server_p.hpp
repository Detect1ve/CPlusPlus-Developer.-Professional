#ifndef SERVER_P_HPP
#define SERVER_P_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <database.hpp>
#include <server.hpp>

class ServerImpl
{
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
private:
    void do_accept();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
};

#endif // SERVER_P_HPP
