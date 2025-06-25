#pragma once

#include <boost/asio.hpp>

#include <async.h>

namespace async
{
    class Session;
    class Server;

    class Server
    {
    public:
        Server(
            boost::asio::io_context& io_context,
            std::uint16_t            port,
            std::size_t              bulk_size);

        ~Server();

    private:
        void start_accept();
        void handle_accept(
            std::shared_ptr<Session>         new_session,
            const boost::system::error_code& error);

        boost::asio::io_context& io_context_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::size_t bulk_size_;
        std::vector<std::shared_ptr<Session>> sessions_;
    };

    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(boost::asio::io_context& io_context, std::size_t bulk_size);
        ~Session();

        auto socket() -> boost::asio::ip::tcp::socket&;
        void start();

    private:
        void handle_read(
            const boost::system::error_code& error,
            std::size_t                      bytes_transferred);

        boost::asio::ip::tcp::socket socket_;
        std::size_t bulk_size_;
        handle_t handle_;
        boost::asio::streambuf buffer_;
    };
} // namespace async
