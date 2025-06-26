#include <iostream>

#include <server.h>

auto main(
    const int   argc,
    const char* argv[]) -> int
{
    int ret = 0;

    try
    {
        short port = 0;

        if (argc != 2)
        {
            std::cerr << "Usage: join_server <port>" << std::endl;
            ret = -1;

            return ret;
        }

        auto [ptr, ec] = std::from_chars(argv[1], argv[1] + strlen(argv[1]), port);
        if (ec != std::errc{})
        {
            std::cerr << "Invalid port format" << std::endl;
            ret = -2;

            return ret;
        }

        boost::asio::io_context io_context;

        Server server(io_context, port);

        std::cout << "Server started on port " << port << std::endl;

        io_context.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        ret = -3;

        return ret;
    }

    return ret;
}
