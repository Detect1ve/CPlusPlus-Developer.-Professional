#ifndef CUSTOM_PRINT_HPP
#define CUSTOM_PRINT_HPP

#include <version>

#ifdef __cpp_lib_print
#include <print>
#else
#include <format>
#include <iostream>
#endif

#ifdef _WIN32
#include <io.h>
#define WRITE_FUNC _write
#else
#include <unistd.h>
#define WRITE_FUNC write
#endif

namespace wrapper
{
    inline int fileno(std::FILE *const stream)
    {
#ifdef _WIN32
        return ::_fileno(stream);
#else
        return ::fileno(stream);
#endif
    }

    inline void write_raw(const std::string_view message) noexcept
    {
        if (message.empty())
        {
            return;
        }

        std::ignore = WRITE_FUNC(wrapper::fileno(stderr), message.data(),
            static_cast<unsigned>(message.size()));
    }
} // namespace wrapper

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
    void println(std::format_string<Args...> fmt, Args&&... args)
    {
        cp::println(stdout, fmt, std::forward<Args>(args)...);
    }

    inline void safe_error(const std::string_view message) noexcept
    {
        wrapper::write_raw("Fatal error: ");

        if (!message.empty())
        {
            wrapper::write_raw(message);
        }
        else
        {
            wrapper::write_raw("Unknown error");
        }

        wrapper::write_raw("\n");
    }

    inline void safe_error() noexcept
    {
        safe_error({});
    }
} // namespace cp

#endif // CUSTOM_PRINT_HPP
