#include <iostream>
#include <ranges>

#include <server.hpp>

int main(
    const int   argc,
    const char* argv[])
{
    int ret = 0;

    try
    {
        enum : unsigned char
        {
            BASE = 10
        };
        auto const args = std::span(argv, argc) | std::views::transform(
            [](char const *const arg)
            {
                return std::string_view(arg);
            });
        int16_t port = 0;

        if (args.size() != 2)
        {
            std::cerr << "Usage: " << args[0] << " <port>\n";
            ret = -1;

            return ret;
        }

        auto [ptr, ec] = std::from_chars(args[1].data(), args[1].data() + args[1].size(),
            port, BASE);
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
