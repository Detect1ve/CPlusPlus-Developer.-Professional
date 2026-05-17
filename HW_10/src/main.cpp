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

#include <gsl/util>

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
            cp::println(stderr, "Usage: {} <port> <bulk_size>", gsl::at(args, 0));
            ret = -1;

            return ret;
        }

        if (auto port_arg = gsl::at(args, 1); std::from_chars(port_arg.data(),
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            port_arg.data() + port_arg.size(), port, BASE).ec != std::errc{})
        {
            cp::println(stderr, "Invalid port format");
            ret = -2;

            return ret;
        }
        if (auto size_arg = gsl::at(args, 2); std::from_chars(size_arg.data(),
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            size_arg.data() + size_arg.size(), bulk_size, BASE).ec != std::errc{})
        {
            cp::println(stderr, "Invalid bulk_size format");
            ret = -3;

            return ret;
        }

        async::Server server(port, bulk_size);

        server.setup_signal_handling();
        server.run();
    }
    catch (const std::exception& e)
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
