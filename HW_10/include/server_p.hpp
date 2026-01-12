#ifndef SERVER_P_HPP
#define SERVER_P_HPP

#include <iostream>

#include <boost/asio.hpp>

#include <server.hpp>

namespace async
{
    class ServerImpl
    {
    public:
        ServerImpl(
            std::unique_ptr<boost::asio::io_context> io_context,
            const std::uint16_t                      port,
            const std::size_t                        bulk_size)
            :
            io_context_(std::move(io_context)),
            acceptor_(*io_context_,
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
            bulk_size_(bulk_size)
        {
            std::cout << "Server started on port " << port << " with bulk size "
                << bulk_size << '\n';
        }
        ~ServerImpl() = default;
        ServerImpl(const ServerImpl&) = delete;
        ServerImpl& operator=(const ServerImpl&) = delete;
        ServerImpl(ServerImpl&&) = delete;
        ServerImpl& operator=(ServerImpl&&) = delete;

        void start_accept()
        {
            acceptor_.async_accept(
                [this](
                    const boost::system::error_code& error,
                    boost::asio::ip::tcp::socket     socket)
                {
                    if (!error)
                    {
                        auto new_session = std::make_shared<Session>(&socket, *this,
                            bulk_size_);

                        sessions_.emplace_back(new_session);
                        new_session->start();
                    }
                    else
                    {
                        std::cerr << "Accept error: " << error.message() << '\n';
                    }

                    start_accept();
                });
        }

        void remove_session(const std::shared_ptr<Session>& session)
        {
            std::erase(sessions_, session);
        }

    private:
        friend class Server;
        std::unique_ptr<boost::asio::io_context> io_context_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::size_t bulk_size_;
        std::vector<std::shared_ptr<Session>> sessions_;
    };

    class SessionImpl
    {
    public:
        SessionImpl(
            boost::asio::ip::tcp::socket& socket,
            ServerImpl&                   server,
            const std::size_t             bulk_size,
            Session&                      session)
            :
            socket_(std::move(socket)),
            bulk_size_(bulk_size),
            server_(server),
            session_(session),
            buffer_(std::make_unique<boost::asio::streambuf>()) {}

        ~SessionImpl()
        {
            if (handle_ != nullptr)
            {
                disconnect(handle_);
            }
        }

        SessionImpl(const SessionImpl&) = delete;
        SessionImpl& operator=(const SessionImpl&) = delete;
        SessionImpl(SessionImpl&&) = delete;
        SessionImpl& operator=(SessionImpl&&) = delete;

        void start()
        {
            handle_ = connect(bulk_size_);
            boost::asio::async_read_until(socket_, *buffer_, '\n',
                [this](
                    const boost::system::error_code& error,
                    const std::size_t                bytes_transferred)
                {
                    handle_read(error, bytes_transferred);
                });
        }

        void handle_read(
            const boost::system::error_code& error,
            const std::size_t                bytes_transferred)
        {
            if (!error)
            {
                std::string data;

                data.resize(bytes_transferred);
                boost::asio::buffer_copy(boost::asio::buffer(data), buffer_->data(),
                    bytes_transferred);
                buffer_->consume(bytes_transferred);
                receive(handle_, data.c_str(), data.size());
                boost::asio::async_read_until(socket_, *buffer_, '\n',
                    [this](
                        const boost::system::error_code& new_error,
                        const std::size_t                new_bytes_transferred)
                    {
                        handle_read(new_error, new_bytes_transferred);
                    });
            }
            else if (error == boost::asio::error::eof)
            {
                if (buffer_->size() > 0)
                {
                    std::string data;

                    data.resize(buffer_->size());
                    boost::asio::buffer_copy(boost::asio::buffer(data), buffer_->data(),
                        buffer_->size());
                    buffer_->consume(buffer_->size());
                    if (!data.empty() && data.back() != '\n')
                    {
                        data += '\n';
                    }

                    receive(handle_, data.c_str(), data.size());
                }

                if (handle_ != nullptr)
                {
                    disconnect(handle_);
                    handle_ = nullptr;
                }

                socket_.close();
                server_.remove_session(session_.shared_from_this());
            }
            else
            {
                std::cerr << "Error: " << error.message() << '\n';
            }
        }

    private:
        boost::asio::ip::tcp::socket socket_;
        std::size_t bulk_size_;
        handle_t handle_{nullptr};
        ServerImpl& server_;
        Session& session_;
        std::unique_ptr<boost::asio::streambuf> buffer_;
    };
} // namespace async

#endif // SERVER_P_HPP
