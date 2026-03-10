#include <cstddef> // std::size_t
#include <cstdio> // stderr
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <span> // std::span
#include <utility>

#include <bayan.hpp>
#include <custom_print.hpp>

int main(
    const int          argc,
    const char **const argv)
{
    const std::span<const char* const> args(argv, static_cast<std::size_t>(argc));
    try
    {
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
            cp::println(stderr, "process_files return {}", +std::to_underlying(status));
        }

        return std::to_underlying(status);
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error();

        return EXIT_FAILURE;
    }
}
