#include <boost/asio.hpp>

#include <server_p.hpp>

namespace async
{
    Server::Server(
        const std::uint16_t port,
        const std::size_t   bulk_size)
        :
        pimpl_(std::make_unique<ServerImpl>(std::make_unique<boost::asio::io_context>(),
            port, bulk_size))
    {
        pimpl_->start_accept();
    }

    Server::~Server() = default;

    void Server::run()
    {
        pimpl_->io_context_->run();
    }

    void Server::stop()
    {
        pimpl_->io_context_->stop();
    }

    Session::Session(
        void*             socket,
        ServerImpl&       server,
        const std::size_t bulk_size)
        :
        pimpl_(std::make_unique<SessionImpl>(
            *static_cast<boost::asio::ip::tcp::socket*>(socket),
            server,
            bulk_size,
            *this)) { }

    Session::~Session() = default;

    void Session::start()
    {
        pimpl_->start();
    }
} // namespace async
