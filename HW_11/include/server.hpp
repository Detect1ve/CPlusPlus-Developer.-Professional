#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

#include <database.hpp>

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(
        boost::asio::ip::tcp::socket socket,
        Database*                    database);

    void start();

private:
    void do_read();

    void do_write(std::string_view message);
    void do_write(const std::vector<std::string>& messages);

    void process_command(std::string_view command);

    boost::asio::ip::tcp::socket socket_;
    Database* database_;
    std::unique_ptr<boost::asio::streambuf> buffer_;
};

class Server
{
public:
    Server(
        boost::asio::io_context& io_context,
        int16_t                  port);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
};

#endif /* SERVER_HPP */
