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

    private:
        std::unique_ptr<ServerImpl> pimpl_;
    };

    class SessionImpl;

    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(
            void*             socket,
            class ServerImpl& server,
            std::size_t       bulk_size);
        ~Session();

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;
        Session(Session&&) = delete;
        Session& operator=(Session&&) = delete;

        void start();

    private:
        std::unique_ptr<SessionImpl> pimpl_;
    };
} // namespace async

#endif /* SERVER_HPP */
