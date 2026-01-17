#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <fcntl.h>

#include <gsl/pointers>

namespace wrapper
{
    inline int close(const int file_desc)
    {
#ifdef _WIN32
        return ::_close(file_desc);
#else
        return ::close(file_desc);
#endif
    }

    inline int dup2(
        const int oldfd,
        const int newfd)
    {
#ifdef _WIN32
        return ::_dup2(oldfd, newfd);
#else
        return ::dup2(oldfd, newfd);
#endif
    }

    inline int fileno(std::FILE *const stream)
    {
#ifdef _WIN32
        return ::_fileno(stream);
#else
        return ::fileno(stream);
#endif
    }
} // namespace wrapper

class StreamCapture
{
    int _fd_old = -1;
    int _target_fd;
    std::array<int, 2> _pipefd = {{ -1, -1 }};
    std::FILE *_target_stream;
    std::mutex _data_mutex;
    std::string _captured_data;
    std::thread _reader_thread;
    static constexpr std::uint8_t pipe_r = 0;
    static constexpr std::uint8_t pipe_w = 1;
    static constexpr std::uint16_t PIPE_SIZE = 4096;

    std::error_code cleanup() noexcept
    {
        std::error_code er_code;

        if (_fd_old != -1)
        {
            if (wrapper::dup2(_fd_old, _target_fd) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            if (wrapper::close(_fd_old) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            _fd_old = -1;
        }

        if (_pipefd[pipe_w] != -1)
        {
            if (wrapper::close(_pipefd[pipe_w]) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            _pipefd[pipe_w] = -1;
        }

        if (_reader_thread.joinable())
        {
            _reader_thread.join();
        }

        if (_pipefd[pipe_r] != -1)
        {
            if (wrapper::close(_pipefd[pipe_r]) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            _pipefd[pipe_r] = -1;
        }

        return er_code;
    }

    void reader_worker()
    {
        std::array<char, PIPE_SIZE> buffer{};

        while (true)
        {
#ifdef _WIN32
            const int bytes_read = _read(_pipefd[pipe_r], buffer.data(),
                static_cast<unsigned>(buffer.size()));
#else
            const ssize_t bytes_read = read(_pipefd[pipe_r], buffer.data(),
                buffer.size());
#endif
            if (bytes_read > 0)
            {
                const std::scoped_lock<std::mutex> lock(_data_mutex);
                _captured_data.append(buffer.data(),
                    static_cast<std::string::size_type>(bytes_read));
            }
            else if (bytes_read == 0 || errno != EINTR)
            {
                break;
            }
        }
    }

public:
    explicit StreamCapture(FILE *const target_stream)
    :
    _target_fd(wrapper::fileno(target_stream)),
    _target_stream(target_stream)
    {
#ifdef _WIN32
        if (_pipe(_pipefd.data(), PIPE_SIZE, _O_BINARY | _O_NOINHERIT) == -1)
#else
        if (pipe2(_pipefd.data(), O_CLOEXEC) == -1)
#endif
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (fflush(_target_stream) == EOF)
        {
            const int err = errno;

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }
#ifdef _WIN32
        _fd_old = _dup(_target_fd);
#else
        _fd_old = fcntl(_target_fd, F_DUPFD_CLOEXEC, 0);
#endif
        if (_fd_old == -1)
        {
            const int err = errno;

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }

        if (wrapper::dup2(_pipefd[pipe_w], _target_fd) == -1)
        {
            const int err = errno;

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }

        try
        {
            _reader_thread = std::thread(&StreamCapture::reader_worker, this);
        }
        catch (...)
        {
            auto err_cleanup = cleanup();

            throw std::system_error(err_cleanup);
        }
    }

    ~StreamCapture()
    {
        if (_fd_old != -1)
        {
            try
            {
                End();
            }
            catch (...)
            {
                // @TODO
            }
        }
    }

    StreamCapture(const StreamCapture&) = delete;
    StreamCapture& operator=(const StreamCapture&) = delete;
    StreamCapture(StreamCapture&&) = delete;
    StreamCapture& operator=(StreamCapture&&) = delete;

    std::string End()
    {
        std::error_code ec_cleanup;
        std::error_code ec_fflush;

        if (_fd_old == -1)
        {
            return _captured_data;
        }

        if (fflush(_target_stream) == EOF)
        {
            ec_fflush = std::error_code(errno, std::generic_category());
        }

        ec_cleanup = cleanup();

        if (ec_cleanup)
        {
            throw std::system_error(ec_cleanup, "cleanup failed during End()");
        }

        if (ec_fflush)
        {
            throw std::system_error(ec_fflush, "fflush failed during End()");
        }

        const std::scoped_lock<std::mutex> lock(_data_mutex);

        return std::move(_captured_data);
    }
};

template <auto GetStream>
class BaseCapture
{
    static gsl::owner<StreamCapture*>* instance()
    {
        static gsl::owner<StreamCapture*> s_instance = nullptr;

        return &s_instance;
    }

    static void Destroy()
    {
        gsl::owner<StreamCapture*> inst = *instance();
        if (inst != nullptr)
        {
            *instance() = nullptr;

            delete inst;
        }
    }
public:
    static void Begin()
    {
        auto *inst = instance();
        if (*inst == nullptr)
        {
            *inst = new StreamCapture(GetStream());

            if (std::atexit(Destroy) != 0)
            {
                throw std::system_error(
                    std::make_error_code(std::errc::not_enough_memory));
            }
        }
    }

    static std::string End()
    {
        std::string output;

        if (*instance() == nullptr)
        {
            return output;
        }

        output = (*instance())->End();
        Destroy();

        return output;
    }
};

using StdoutCapture = BaseCapture<[] { return stdout; }>;
using StderrCapture = BaseCapture<[] { return stderr; }>;

#endif // CAPTURE_HPP
