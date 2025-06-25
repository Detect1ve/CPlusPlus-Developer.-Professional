#include <iostream>

#include <server.h>

auto main(
    const int   argc,
    const char *argv[]) -> int
{
    int ret = 0;

    try
    {
        uint16_t port = 0;
        std::size_t bulk_size = 0;

        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <port> <bulk_size>" << std::endl;
            ret = -1;

            return ret;
        }

        {
            auto [ptr, ec] = std::from_chars(argv[1], argv[1] + strlen(argv[1]), port);
            if (ec != std::errc{})
            {
                std::cerr << "Invalid port format" << std::endl;
                ret = -2;

                return ret;
            }
        }

        {
            auto [ptr, ec] = std::from_chars(argv[2], argv[2] + strlen(argv[2]),
                bulk_size);
            if (ec != std::errc{})
            {
                std::cerr << "Invalid bulk_size format" << std::endl;
                ret = -3;

                return ret;
            }
        }

        boost::asio::io_context io_context;
        async::Server server(io_context, port, bulk_size);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return ret;
}
