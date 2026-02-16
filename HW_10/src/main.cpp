#include <charconv>
#include <cstddef> // std::size_t
#include <cstdint> // std::uint16_t
#include <cstdio> // stderr
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <ranges>
#include <span> // std::span
#include <string_view> // std::string_view
#include <system_error> // std::errc

#include <custom_print.hpp>
#include <server.hpp>

int main(
    const int   argc,
    const char *argv[])
{
    constexpr unsigned char BASE = 10;
    int ret = 0;

    try
    {
        const auto args = std::span(argv, static_cast<std::size_t>(argc))
            | std::views::transform([](const char *const arg) noexcept
            {
                return std::string_view(arg);
            });
        std::uint16_t port = 0;
        std::size_t bulk_size = 0;

        if (args.size() != 3)
        {
            cp::println(stderr, "Usage: {} <port> <bulk_size>", args[0]);
            ret = -1;

            return ret;
        }

        {
            auto [ptr, ec] = std::from_chars(args[1].data(),
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                args[1].data() + args[1].size(), port, BASE);
            if (ec != std::errc{})
            {
                cp::println(stderr, "Invalid port format");
                ret = -2;

                return ret;
            }
        }

        {
            auto [ptr, ec] = std::from_chars(args[2].data(),
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                args[2].data() + args[2].size(), bulk_size, BASE);
            if (ec != std::errc{})
            {
                cp::println(stderr, "Invalid bulk_size format");
                ret = -3;

                return ret;
            }
        }

        async::Server server(port, bulk_size);

        server.setup_signal_handling();
        server.run();
    }
    catch (std::exception& e)
    {
        cp::safe_error(e.what());
        ret = -4;
    }
    catch (...)
    {
        cp::safe_error(nullptr);
        ret = EXIT_FAILURE;
    }

    return ret;
}
