#if !defined(_WIN32) && !defined(_MSC_VER)
#include <csignal>
#endif
#include <cstddef> // std::size_t
#include <cstdint> // std::uint16_t
#include <memory> // std::make_unique

#include <boost/asio/io_context.hpp> // boost::asio::io_context
#include <boost/asio/ip/tcp.hpp> // boost::asio::ip::tcp

#if !defined(_WIN32) && !defined(_MSC_VER)
#include <custom_print.hpp> // cp::println
#endif
#include <server.hpp>
#include <server_p.hpp>

namespace
{
    [[nodiscard]] async::Server*& get_server_instance() noexcept
    {
        static async::Server* instance = nullptr; // NOLINT(misc-const-correctness)

        return instance;
    }

#if !defined(_WIN32) && !defined(_MSC_VER)
    void signal_handler(const int /*signal*/)
    {
        if (auto* instance = get_server_instance(); instance)
        {
            instance->stop();
        }
    }
#endif
} // namespace

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

    void Server::setup_signal_handling()
    {
        get_server_instance() = this;
#if !defined(_WIN32) && !defined(_MSC_VER)
// NOLINTBEGIN(misc-include-cleaner)
        struct sigaction sigaction_info {};

        sigaction_info.sa_handler = signal_handler;
        sigemptyset(&sigaction_info.sa_mask);
        sigaction_info.sa_flags = 0;
        if (sigaction(SIGHUP, &sigaction_info, nullptr) == -1)
        {
            cp::println(stderr, "Failed to register SIGHUP handler, but continue anyway");
        }

        if (sigaction(SIGINT, &sigaction_info, nullptr) == -1)
        {
            cp::println(stderr, "Failed to register SIGINT handler, but continue anyway");
        }

        if (sigaction(SIGQUIT, &sigaction_info, nullptr) == -1)
// NOLINTEND(misc-include-cleaner)
        {
            cp::println(stderr,
                "Failed to register SIGQUIT handler, but continue anyway");
        }

        if (sigaction(SIGTERM, &sigaction_info, nullptr) == -1)
        {
            cp::println(stderr,
                "Failed to register SIGTERM handler, but continue anyway");
        }
#endif
    }

    Session::Session(
        void *const       socket,
        ServerImpl&       server,
        const std::size_t bulk_size)
        :
        pimpl_(nullptr)
    {
        pimpl_  = std::make_unique<SessionImpl>(
            *static_cast<boost::asio::ip::tcp::socket*>(socket), server, bulk_size,
            *this);
    }

    void Session::start()
    {
        pimpl_->start();
    }
} // namespace async
