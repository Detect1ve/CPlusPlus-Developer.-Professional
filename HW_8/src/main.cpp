#include <iostream>
#include <utility>

#include <bayan.hpp>

int main(
    const int          argc,
    const char **const argv)
{
    const std::span<const char* const> args(argv, static_cast<std::size_t>(argc));
    auto [status, options] = option_process(args);

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
