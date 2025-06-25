#include <iostream>

#include <boost/bind/bind.hpp>

#include <server.h>

using namespace boost::placeholders;

namespace async
{
    Session::Session(
        boost::asio::io_context& io_context,
        std::size_t              bulk_size)
        :
        socket_(io_context),
        bulk_size_(bulk_size),
        handle_(nullptr) {}

    Session::~Session()
    {
        if (handle_ != nullptr)
        {
            disconnect(handle_);
        }
    }

    auto Session::socket() -> boost::asio::ip::tcp::socket&
    {
        return socket_;
    }

    void Session::start()
    {
        handle_ = connect(bulk_size_);

        boost::asio::async_read_until(socket_, buffer_, '\n',
            [self = shared_from_this()](
                const boost::system::error_code& error,
                const std::size_t                bytes_transferred)
            {
                self->handle_read(error, bytes_transferred);
            }
        );
    }

    void Session::handle_read(
        const boost::system::error_code& error,
        const std::size_t                bytes_transferred)
    {
        if (!error)
        {
            std::string data;
            data.resize(bytes_transferred);

            boost::asio::buffer_copy(boost::asio::buffer(data), buffer_.data(),
                bytes_transferred);

            buffer_.consume(bytes_transferred);

            receive(handle_, data.c_str(), data.size());

            boost::asio::async_read_until(socket_, buffer_, '\n',
                [self = shared_from_this()](
                    const boost::system::error_code& error,
                    const std::size_t                bytes_transferred)
                {
                    self->handle_read(error, bytes_transferred);
                }
            );
        }
        else if (error == boost::asio::error::eof)
        {
            if (buffer_.size() > 0)
            {
                std::string data;
                data.resize(buffer_.size());

                boost::asio::buffer_copy(boost::asio::buffer(data), buffer_.data(),
                    buffer_.size());
                buffer_.consume(buffer_.size());

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
        }
        else
        {
            std::cerr << "Error: " << error.message() << std::endl;
        }
    }

    Server::Server(
        boost::asio::io_context& io_context,
        const std::uint16_t      port,
        const std::size_t        bulk_size)
        :
        io_context_(io_context),
        acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
            port)),
        bulk_size_(bulk_size)
    {
        std::cout << "Server started on port " << port << " with bulk size " << bulk_size
            << std::endl;
        start_accept();
    }

    Server::~Server()
    {
        for (auto& session : sessions_)
        {
            if (session)
            {
                try
                {
                    session->socket().close();
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error closing socket: " << e.what() << std::endl;
                }
            }
        }
    }

    void Server::start_accept()
    {
        auto new_session = std::make_shared<Session>(io_context_, bulk_size_);

        acceptor_.async_accept(new_session->socket(),
            [this, new_session](const boost::system::error_code& error)
            {
                handle_accept(new_session, error);
            }
        );
    }

    void Server::handle_accept(
        std::shared_ptr<Session>         new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            sessions_.emplace_back(new_session);
            new_session->start();
        }
        else
        {
            std::cerr << "Accept error: " << error.message() << std::endl;
        }

        start_accept();
    }
} // namespace async
