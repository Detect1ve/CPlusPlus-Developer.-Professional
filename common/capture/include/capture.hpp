#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <fcntl.h>

#include <gsl/pointers>

class StreamCapture
{
    constexpr std::uint8_t pipe_r = 0;
    constexpr std::uint8_t pipe_w = 1;
    int _fd_old = -1;
    int _target_fd;
    std::array<int, 2> _pipefd = {-1, -1};

    void flush_target() const
    {
        FILE* stream = nullptr;

        if (_target_fd == STDOUT_FILENO)
        {
            stream = stdout;
        }
        else if (_target_fd == STDERR_FILENO)
        {
            stream = stderr;
        }

        if (fflush(stream) == EOF)
        {
            throw std::system_error(errno, std::generic_category());
        }
    }

public:
    explicit StreamCapture(const int target_fd) : _target_fd(target_fd)
    {
        if (pipe2(_pipefd.data(), O_CLOEXEC | O_NONBLOCK) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        flush_target();

        _fd_old = fcntl(_target_fd, F_DUPFD_CLOEXEC, 0);
        if (_fd_old == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (dup2(_pipefd[pipe_w], _target_fd) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }
    }

    ~StreamCapture()
    {
        if (_fd_old != -1)
        {
            dup2(_fd_old, _target_fd);
            close(_fd_old);
            _fd_old = -1;
        }

        if (_pipefd[pipe_r] != -1)
        {
            close(_pipefd[pipe_r]);
            _pipefd[pipe_r] = -1;
        }

        if (_pipefd[pipe_w] != -1)
        {
            close(_pipefd[pipe_w]);
            _pipefd[pipe_w] = -1;
        }
    }

    StreamCapture(const StreamCapture&) = delete;
    StreamCapture& operator=(const StreamCapture&) = delete;
    StreamCapture(StreamCapture&&) = delete;
    StreamCapture& operator=(StreamCapture&&) = delete;

    std::string End()
    {
        constexpr std::uint16_t BUFFER_SIZE = 1024;
        ssize_t bytes_read = 0;
        std::array<char, BUFFER_SIZE> buffer{};
        std::string out;

        flush_target();

        if (dup2(_fd_old, _target_fd) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (close(_fd_old) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        _fd_old = -1;

        if (close(_pipefd[pipe_w]) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        _pipefd[pipe_w] = -1;

        while ((bytes_read = read(_pipefd[pipe_r], buffer.data(), sizeof(buffer))) > 0)
        {
            out += std::string(buffer.data(),
                static_cast<std::string::size_type>(bytes_read));
        }

        if (  bytes_read == -1
           && errno != EAGAIN
           && errno != EWOULDBLOCK
           && errno != EINTR)
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (close(_pipefd[pipe_r]) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        _pipefd[pipe_r] = -1;

        return out;
    }
};

template <int TargetFd>
class BaseCapture
{
    static void Destroy()
    {
        gsl::owner<StreamCapture*> to_delete = *instance();
        if (to_delete != nullptr)
        {
            *instance() = nullptr;

            delete to_delete;
        }
    }

    static gsl::owner<StreamCapture*>* instance()
    {
        static gsl::owner<StreamCapture*> s_instance = nullptr;

        return &s_instance;
    }
public:
    static void Begin()
    {
        if (*instance() == nullptr)
        {
            *instance() = new StreamCapture(TargetFd);
            (void)std::atexit(Destroy);
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

using StdoutCapture = BaseCapture<STDOUT_FILENO>;
using StderrCapture = BaseCapture<STDERR_FILENO>;

#endif // CAPTURE_HPP
