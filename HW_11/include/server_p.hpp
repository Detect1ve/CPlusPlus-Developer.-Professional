#ifndef SERVER_P_HPP
#define SERVER_P_HPP

#include <database.hpp>
#include <server.hpp>
#include <wrapper_boost_asio.hpp>

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
    void run();
    void stop();
};

#endif // SERVER_P_HPP
