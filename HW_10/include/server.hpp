#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>

#include <async.h>

namespace async
{
    class ServerImpl;
    class Session;

    class Server
    {
        std::unique_ptr<ServerImpl> pimpl_;
    public:
        Server(
            std::uint16_t port,
            std::size_t   bulk_size);

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        ~Server();

        void run();
        void setup_signal_handling();
        void stop();
    };

    class SessionImpl;

    class Session : public std::enable_shared_from_this<Session>
    {
        std::unique_ptr<SessionImpl> pimpl_;
    public:
        Session(
            void*       socket,
            ServerImpl& server,
            std::size_t bulk_size);
        ~Session();

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;
        Session(Session&&) = delete;
        Session& operator=(Session&&) = delete;

        void start();
    };
} // namespace async

#endif // SERVER_HPP
