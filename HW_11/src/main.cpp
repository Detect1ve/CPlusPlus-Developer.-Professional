#if defined(__clang__)\
 || __GNUC__ < 14
#include <charconv>
#endif
#include <cstddef> // std::size_t
#include <cstdint> // std::int16_t
#include <cstdio> // stderr
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
    const char* argv[])
{
    int ret = 0;

    try
    {
        auto const args = std::span(argv, static_cast<std::size_t>(argc))
            | std::views::transform([](char const *const arg) noexcept
            {
                return std::string_view(arg);
            });
        constexpr unsigned char BASE = 10;
        std::int16_t port = 0;

        if (args.size() != 2)
        {
            cp::println(stderr, "Usage: {} <port>", gsl::at(args, 0));
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

        Server server(port);

        server.setup_signal_handling();
        server.run();
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());
        ret = -3;
    }
    catch (...)
    {
        cp::safe_error(nullptr);
        ret = -4;
    }

    return ret;
}
