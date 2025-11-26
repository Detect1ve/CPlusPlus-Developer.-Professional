#if defined(__clang__)\
 || __GNUC__ < 14
#include <charconv>
#endif
#include <iostream>
#include <ranges>

#include <server.hpp>
// NOLINTNEXTLINE(bugprone-exception-escape)
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
            std::cerr << "Usage: " << args[0] << " <port>\n";
            ret = -1;

            return ret;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto [ptr, ec] = std::from_chars(args[1].data(), args[1].data() + args[1].size(),
            port, BASE);
        if (ec != std::errc{})
        {
            std::cerr << "Invalid port format\n";
            ret = -2;

            return ret;
        }

        Server server(port);

        server.setup_signal_handling();
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
        ret = -3;

        return ret;
    }

    return ret;
}
