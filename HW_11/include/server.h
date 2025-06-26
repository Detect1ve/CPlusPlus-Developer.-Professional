#pragma once

#include <boost/asio.hpp>

#include <database.h>

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(
        boost::asio::ip::tcp::socket socket,
        Database&                    database);

    void start();

private:
    void do_read();

    void do_write(std::string_view message);
    void do_write(const std::vector<std::string>& messages);

    void process_command(std::string_view command);

    boost::asio::ip::tcp::socket socket_;
    Database& database_;
    boost::asio::streambuf buffer_;
};

class Server
{
public:
    Server(
        boost::asio::io_context& io_context,
        short                    port);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
};
