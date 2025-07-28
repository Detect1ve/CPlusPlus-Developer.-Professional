#include <iostream>

#include <server.h>

int main(
    const int   argc,
    const char* argv[])
{
    int ret = 0;

    try
    {
        short port = 0;

        if (argc != 2)
        {
            std::cerr << "Usage: join_server <port>\n";
            ret = -1;

            return ret;
        }

        auto [ptr, ec] = std::from_chars(argv[1], argv[1] + strlen(argv[1]), port);
        if (ec != std::errc{})
        {
            std::cerr << "Invalid port format\n";
            ret = -2;

            return ret;
        }

        boost::asio::io_context io_context;

        const Server server(io_context, port);

        std::cout << "Server started on port " << port << '\n';

        io_context.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
        ret = -3;

        return ret;
    }

    return ret;
}
