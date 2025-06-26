#include <iostream>

#include <server.h>

using boost::asio::ip::tcp;

Session::Session(
    tcp::socket socket,
    Database&   database)
    :
    socket_(std::move(socket)),
    database_(database) {}

void Session::start()
{
    do_read();
}

void Session::do_read()
{
    auto self(shared_from_this());

    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](
            boost::system::error_code ec,
            std::size_t               /*length*/)
        {
            if (!ec)
            {
                std::string command;
                std::istream is(&buffer_);
                std::getline(is, command);

                process_command(command);

                do_read();
            }
            else if (ec != boost::asio::error::eof)
            {
                std::cerr << "Error: " << ec.message() << std::endl;
            }
        });
}

void Session::do_write(std::string_view message)
{
    auto self(shared_from_this());

    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [self](
            boost::system::error_code ec,
            std::size_t               /*length*/)
        {
            if (ec)
            {
                std::cerr << "Error: " << ec.message() << std::endl;
            }
        });
}

void Session::do_write(const std::vector<std::string>& messages)
{
    for (const auto& message : messages)
    {
        do_write(message + "\n");
    }

    do_write("OK\n");
}

void Session::process_command(std::string_view command)
{
    std::istringstream iss((std::string(command)));
    std::string cmd;

    iss >> cmd;

    if (cmd == "INSERT")
    {
        int id;
        std::string name;
        std::string table;

        iss >> table >> id;

        std::getline(iss >> std::ws, name);

        if (database_.insert(table, id, name))
        {
            do_write("OK\n");
        }
        else
        {
            do_write("ERR duplicate " + std::to_string(id) + "\n");
        }
    }
    else if (cmd == "TRUNCATE")
    {
        std::string table;

        iss >> table;

        database_.truncate(table);
        do_write("OK\n");
    }
    else if (cmd == "INTERSECTION")
    {
        auto result = database_.intersection();

        do_write(result);
    }
    else if (cmd == "SYMMETRIC_DIFFERENCE")
    {
        auto result = database_.symmetric_difference();

        do_write(result);
    }
    else
    {
        do_write("ERR unknown command\n");
    }
}

Server::Server(
    boost::asio::io_context& io_context,
    short                    port)
    :
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    do_accept();
}

void Server::do_accept()
{
    acceptor_.async_accept(
        [this](
            boost::system::error_code ec,
            tcp::socket               socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(std::move(socket), database_)->start();
            }

            do_accept();
        });
}
