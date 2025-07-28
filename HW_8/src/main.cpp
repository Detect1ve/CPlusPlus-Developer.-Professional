#include <iostream>
#if defined(__clang__) or __GNUC__ < 14
#include <utility>
#endif

#include <bayan.hpp>

int main(
    const int          argc,
    const char **const argv)
{
    auto [status, options] = option_process({argv, static_cast<std::size_t>(argc)});

    if (status == ProcessStatus::HELP_REQUESTED)
    {
        return std::to_underlying(ProcessStatus::SUCCESS);
    }

    if (status != ProcessStatus::SUCCESS)
    {
        return std::to_underlying(status);
    }

    status = process_files(options);
    if (status != ProcessStatus::SUCCESS)
    {
        std::cerr << "process_files return " << +std::to_underlying(status) << '\n';
    }

    return std::to_underlying(status);
}
