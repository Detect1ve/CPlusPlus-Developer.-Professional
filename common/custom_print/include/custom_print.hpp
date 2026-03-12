#ifndef CUSTOM_PRINT_HPP
#define CUSTOM_PRINT_HPP

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <version>

#ifdef __cpp_lib_print
#include <print>
#else
#include <format>
#include <iostream>
#endif

namespace cp
{
    template <typename... Args>
    void print(
        std::FILE *const            stream,
        std::format_string<Args...> fmt,
        Args&&...                   args)
    {
#ifdef __cpp_lib_print
        std::print(stream, fmt, std::forward<Args>(args)...);
#else
        if (stream == stdout)
        {
            std::cout << std::format(fmt, std::forward<Args>(args)...);
        }
        else
        {
            std::cerr << std::format(fmt, std::forward<Args>(args)...);
        }
#endif
    }

    template <typename... Args>
    void print(
        std::format_string<Args...> fmt,
        Args&&...                   args)
    {
        cp::print(stdout, fmt, std::forward<Args>(args)...);
    }

    inline void println()
    {
#ifdef __cpp_lib_print
        std::println();
#else
        std::cout << '\n';
#endif
    }

    template <typename... Args>
    void println(
        std::FILE *const            stream,
        std::format_string<Args...> fmt,
        Args&&...                   args)
    {
#ifdef __cpp_lib_print
        std::println(stream, fmt, std::forward<Args>(args)...);
#else
        if (stream == stdout)
        {
            std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
        }
        else
        {
            std::cerr << std::format(fmt, std::forward<Args>(args)...) << '\n';
        }
#endif
    }

    template <typename... Args>
    void println(
        std::format_string<Args...> fmt,
        Args&&...                   args)
    {
        cp::println(stdout, fmt, std::forward<Args>(args)...);
    }

    inline void safe_error(const char *const message) noexcept
    {
        auto write_out = [](const std::string_view msg) noexcept -> bool
        {
            if (msg.empty())
            {
                return true;
            }
#ifdef _WIN32
            return ::_write(::_fileno(stderr), msg.data(),
                static_cast<unsigned>(msg.size())) == static_cast<int>(msg.size());
#else
            return ::write(::fileno(stderr), msg.data(), msg.size())
                == static_cast<ssize_t>(msg.size());
#endif
        };

        if (message != nullptr)
        {
            if (!write_out("Fatal error: "))
            {
                return;
            }

            if (!write_out(message))
            {
                return;
            }

            if (std::fputc('\n', stderr) == EOF)
            {
                return;
            }
        }
        else
        {
            if (!write_out("Unknown error\n"))
            {
                return;
            }
        }
    }
} // namespace cp

#endif // CUSTOM_PRINT_HPP
