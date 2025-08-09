#ifndef SERVER_P_HPP
#define SERVER_P_HPP

#include <boost/asio.hpp>

#include <database.hpp>
#include <server.hpp>

class ServerImpl {
public:
    explicit ServerImpl(std::int16_t port);
    explicit ServerImpl(
        std::promise<std::uint16_t>& port_promise,
        std::int16_t                 port);
    void run();
    void stop();

private:
    void do_accept();

    friend class Server;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
};

#endif // SERVER_P_HPP
