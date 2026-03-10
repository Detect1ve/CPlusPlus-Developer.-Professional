#if __GNUC__ < 14
#include <charconv>
#endif
#include <cstddef> // std::size_t
#include <cstdio> // stderr
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <ranges>
#include <span> // std::span
#include <string_view> // std::string_view
#include <system_error> // std::errc

#include <custom_print.hpp>
#include <taskmanager.hpp>

int main(
    int const          argc,
    const char **const argv)
{
    auto const args = std::span(argv, static_cast<std::size_t>(argc))
        | std::views::transform([](char const *const arg) noexcept
        {
            return std::string_view(arg);
        });
    constexpr int BASE = 10;
    int command_number = 0;
    int ret = 0;

    try
    {
        if (args.size() < 2)
        {
            cp::println(stderr, "Usage: {} <positive_number>", args[0]);
            ret = -1;

            return ret;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (std::from_chars(args[1].data(), args[1].data() + args[1].size(),
            command_number, BASE).ec != std::errc{})
        {
            cp::println(stderr, "Invalid number format");
            ret = -2;

            return ret;
        }

        if (command_number <= 0)
        {
            cp::println(stderr, "Block size must be a positive number");
            ret = -3;

            return ret;
        }

        bulk::taskmanager my_task_manager(command_number, args[0]);

        my_task_manager.setup_signal_handling();
        ret = my_task_manager.run();
        if (ret != 0)
        {
            cp::println(stderr, "run return {}", ret);
        }
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        ret = EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error();

        ret = EXIT_FAILURE;
    }

    return ret;
}
