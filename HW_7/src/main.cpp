#if __GNUC__ < 15
#include <charconv>
#endif
#include <cstring>

#include <taskmanager.hpp>

auto main(
    int const    argc,
    char **const argv) -> int
{
    int command_number;
    int ret = 0;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <positive_number>" << std::endl;
        ret = -1;

        return ret;
    }

    auto [ptr, ec] = std::from_chars(argv[1], argv[1] + strlen(argv[1]), command_number);
    if (ec != std::errc{})
    {
        std::cerr << "Invalid number format" << std::endl;
        ret = -2;

        return ret;
    }

    if (command_number <= 0)
    {
        std::cerr << "Block size must be a positive number" << std::endl;
        ret = -3;

        return ret;
    }

    bulk::taskmanager my_task_manager(command_number, argv[0]);

    ret = my_task_manager.run();
    if (ret != 0)
    {
        std::cerr << "run return " << ret << std::endl;
    }

    return ret;
}
