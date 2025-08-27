#if !defined(_WIN32) && !defined(_MSC_VER)
#include <csignal>
#endif
#include <iostream>

#include <server.hpp>
#include <server_p.hpp>

using boost::asio::ip::tcp;

namespace
{
    [[nodiscard]] Server*& get_server_instance() noexcept
    {
        static Server* instance = nullptr; // NOLINT(misc-const-correctness)

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

class Session;

class SessionImpl
{
    void do_read();
    void do_write(std::string_view message);
    void do_write(const std::vector<std::string>& messages);
    void process_command(std::string_view command);

    tcp::socket socket_;
    Database* database_;
    std::unique_ptr<boost::asio::streambuf> buffer_;
    std::shared_ptr<Session> self_;
public:
    SessionImpl(
        tcp::socket socket,
        Database*   database);
    ~SessionImpl() = default;
    SessionImpl(const SessionImpl&) = delete;
    SessionImpl& operator=(const SessionImpl&) = delete;
    SessionImpl(SessionImpl&&) = delete;
    SessionImpl& operator=(SessionImpl&&) = delete;

    void start(std::shared_ptr<Session> self);
};

class Session : public std::enable_shared_from_this<Session>
{
    std::unique_ptr<SessionImpl> pimpl_;
public:
    Session(
        tcp::socket socket,
        Database*   database)
        :
        pimpl_(std::make_unique<SessionImpl>(std::move(socket), database)) {}

    void start()
    {
        pimpl_->start(shared_from_this());
    }
};

SessionImpl::SessionImpl(
    tcp::socket socket,
    Database*   database)
    :
    socket_(std::move(socket)),
    database_(database),
    buffer_(std::make_unique<boost::asio::streambuf>()) {}

void SessionImpl::start(std::shared_ptr<Session> self)
{
    self_ = std::move(self);
    do_read();
}

void SessionImpl::do_read()
{
    boost::asio::async_read_until(socket_, *buffer_, '\n',
        [this](
            const boost::system::error_code& error_code,
            const std::size_t                length)
        {
            if (!error_code)
            {
                std::string command;

                command.resize(length);
                boost::asio::buffer_copy(boost::asio::buffer(command), buffer_->data(),
                    length);
                buffer_->consume(length);
                process_command(command);
                do_read();
            }
            else if (error_code != boost::asio::error::eof)
            {
                std::cerr << "Error: " << error_code.message() << '\n';
            }
        });
}

void SessionImpl::do_write(std::string_view message)
{
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [](
            const boost::system::error_code& error_code,
            std::size_t                      /*length*/)
        {
            if (error_code)
            {
                std::cerr << "Error: " << error_code.message() << '\n';
            }
        });
}

void SessionImpl::do_write(const std::vector<std::string>& messages)
{
    for (const auto& message : messages)
    {
        do_write(message + '\n');
    }

    do_write("OK\n");
}

void SessionImpl::process_command(std::string_view command)
{
    std::istringstream iss((std::string(command)));
    std::string cmd;

    iss >> cmd;

    if (cmd == "INSERT")
    {
        int record_id = 0;
        std::string name;
        std::string table;

        iss >> table >> record_id;
        std::getline(iss >> std::ws, name);
        if (database_->insert(table, record_id, name))
        {
            do_write("OK\n");
        }
        else
        {
            do_write("ERR duplicate " + std::to_string(record_id) + '\n');
        }
    }
    else if (cmd == "TRUNCATE")
    {
        std::string table;

        iss >> table;
        database_->truncate(table);
        do_write("OK\n");
    }
    else if (cmd == "INTERSECTION")
    {
        auto result = database_->intersection();

        do_write(result);
    }
    else if (cmd == "SYMMETRIC_DIFFERENCE")
    {
        auto result = database_->symmetric_difference();

        do_write(result);
    }
    else
    {
        do_write("ERR unknown command\n");
    }
}

ServerImpl::ServerImpl(const std::uint16_t port)
    :
    acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
    database_()
{
    do_accept();
}

ServerImpl::ServerImpl(
    std::promise<std::uint16_t>& port_promise,
    const std::uint16_t          port)
    :
    acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
    database_()
{
    port_promise.set_value(acceptor_.local_endpoint().port());

    do_accept();
}

void ServerImpl::do_accept()
{
    acceptor_.async_accept(
        [this](
            boost::system::error_code error_code,
            tcp::socket               socket)
        {
            if (!error_code)
            {
                std::make_shared<Session>(std::move(socket), &database_)->start();
            }

            do_accept();
        });
}

void ServerImpl::run()
{
    io_context_.run();
}

void ServerImpl::stop()
{
    io_context_.stop();
}

Server::Server(const std::int16_t port) : pimpl_(std::make_unique<ServerImpl>(port)) {}

Server::Server(
    std::promise<std::uint16_t>& port_promise,
    const std::int16_t           port)
    :
    pimpl_(std::make_unique<ServerImpl>(port_promise, port)) {}

void Server::run()
{
    pimpl_->run();
}

void Server::setup_signal_handling()
{
    get_server_instance() = this;
#if !defined(_WIN32) && !defined(_MSC_VER)
    struct sigaction sigaction_info {};

    sigaction_info.sa_handler = signal_handler;
    sigemptyset(&sigaction_info.sa_mask);
    sigaction_info.sa_flags = 0;
    if (sigaction(SIGHUP, &sigaction_info, nullptr) == -1)
    {
        std::cerr << "Failed to register SIGHUP handler, but continue anyway\n";
    }

    if (sigaction(SIGINT, &sigaction_info, nullptr) == -1)
    {
        std::cerr << "Failed to register SIGINT handler, but continue anyway\n";
    }

    if (sigaction(SIGQUIT, &sigaction_info, nullptr) == -1)
    {
        std::cerr << "Failed to register SIGQUIT handler, but continue anyway\n";
    }

    if (sigaction(SIGTERM, &sigaction_info, nullptr) == -1)
    {
        std::cerr << "Failed to register SIGTERM handler, but continue anyway\n";
    }
#endif
}

void Server::stop()
{
    pimpl_->stop();
}

Server::~Server() = default;
