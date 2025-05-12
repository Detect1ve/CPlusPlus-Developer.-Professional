#include <iostream>

#include <bayan.hpp>

auto main(
    const int    argc,
    char **const argv) -> int
{
    auto [ret, options] = option_process(argc, argv);
    if (ret != 0)
    {
        std::cerr << "option_process return " << ret << std::endl;

        return ret;
    }

    ret = process_files(options);
    if (ret != 0)
    {
        std::cerr << "process_files return " << ret << std::endl;
    }

    return ret;
}
