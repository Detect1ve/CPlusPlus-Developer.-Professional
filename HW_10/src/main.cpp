#include <charconv>
#include <iostream>
#include <ranges>

#include <server.hpp>

int main(
    const int   argc,
    const char *argv[])
{
    enum : unsigned char
    {
        BASE = 10
    };
    int ret = 0;

    try
    {
        const auto args = std::span(argv, argc) | std::views::transform(
            [](const char *const arg)
            {
                return std::string_view(arg);
            });
        uint16_t port = 0;
        std::size_t bulk_size = 0;

        if (args.size() != 3)
        {
            std::cerr << "Usage: " << args[0] << " <port> <bulk_size>\n";
            ret = -1;

            return ret;
        }

        {
            auto [ptr, ec] = std::from_chars(args[1].data(),
                args[1].data() + args[1].size(), port, BASE);
            if (ec != std::errc{})
            {
                std::cerr << "Invalid port format\n";
                ret = -2;

                return ret;
            }
        }

        {
            auto [ptr, ec] = std::from_chars(args[2].data(),
                args[2].data() + args[2].size(), bulk_size, BASE);
            if (ec != std::errc{})
            {
                std::cerr << "Invalid bulk_size format\n";
                ret = -3;

                return ret;
            }
        }

        async::Server server(port, bulk_size);

        server.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }

    return ret;
}
