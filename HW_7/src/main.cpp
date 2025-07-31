#if __GNUC__ < 14
#include <charconv>
#endif
#include <ranges>

#include <taskmanager.hpp>

int main(
    int const    argc,
    char **const argv)
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
    int command_number = 0;
    int ret = 0;

    if (args.size() < 2)
    {
        std::cerr << "Usage: " << args[0] << " <positive_number>\n";
        ret = -1;

        return ret;
    }

    auto [ptr, ec] = std::from_chars(args[1].data(), args[1].data() + args[1].size(),
        command_number, BASE);
    if (ec != std::errc{})
    {
        std::cerr << "Invalid number format\n";
        ret = -2;

        return ret;
    }

    if (command_number <= 0)
    {
        std::cerr << "Block size must be a positive number\n";
        ret = -3;

        return ret;
    }

    bulk::taskmanager my_task_manager(command_number, args[0]);

    ret = my_task_manager.run();
    if (ret != 0)
    {
        std::cerr << "run return " << ret << '\n';
    }

    return ret;
}
