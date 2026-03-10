#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <fcntl.h>

#include <gsl/pointers>

class StreamCapture
{
public:
    explicit StreamCapture(FILE *const target_stream)
    :
    target_fd_(fileno(target_stream)),
    target_stream_(target_stream)
    {
#ifdef _WIN32
        if (_pipe(pipefd_.data(), PIPE_SIZE, _O_BINARY | _O_NOINHERIT) == -1)
#else
        if (pipe2(pipefd_.data(), O_CLOEXEC) == -1)
#endif
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (fflush(target_stream_) == EOF)
        {
            const int err{errno};

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }
#ifdef _WIN32
        fd_old_ = _dup(target_fd_);
#else
        fd_old_ = fcntl(target_fd_, F_DUPFD_CLOEXEC, 0);
#endif
        if (fd_old_ == -1)
        {
            const int err{errno};

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }

        if (dup2(pipefd_[PIPE_W], target_fd_) == -1)
        {
            const int err{errno};

            errno = 0;

            auto err_cleanup = cleanup();

            throw std::system_error(err, std::generic_category(),
                "cleanup() also returned " + err_cleanup.message());
        }

        try
        {
            reader_thread_ = std::thread(&StreamCapture::reader_worker, this);
        }
        catch (...)
        {
            auto err_cleanup = cleanup();

            throw std::system_error(err_cleanup);
        }
    }
    ~StreamCapture()
    {
        if (fd_old_ != -1)
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

        if (fd_old_ == -1)
        {
            return captured_data_;
        }

        if (fflush(target_stream_) == EOF)
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

        const std::scoped_lock<std::mutex> lock(data_mutex_);

        return std::move(captured_data_);
    }

private:
    static int close(const int file_desc)
    {
#ifdef _WIN32
        return ::_close(file_desc);
#else
        return ::close(file_desc);
#endif
    }

    static int dup2(
        const int oldfd,
        const int newfd)
    {
#ifdef _WIN32
        return ::_dup2(oldfd, newfd);
#else
        return ::dup2(oldfd, newfd);
#endif
    }

    static int fileno(std::FILE *const stream)
    {
#ifdef _WIN32
        return ::_fileno(stream);
#else
        return ::fileno(stream);
#endif
    }

    std::error_code cleanup() noexcept
    {
        std::error_code er_code;

        if (fd_old_ != -1)
        {
            if (dup2(fd_old_, target_fd_) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            if (close(fd_old_) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            fd_old_ = -1;
        }

        if (pipefd_[PIPE_W] != -1)
        {
            if (close(pipefd_[PIPE_W]) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            pipefd_[PIPE_W] = -1;
        }

        if (reader_thread_.joinable())
        {
            reader_thread_.join();
        }

        if (pipefd_[PIPE_R] != -1)
        {
            if (close(pipefd_[PIPE_R]) == -1)
            {
                er_code = std::error_code(errno, std::generic_category());
            }

            pipefd_[PIPE_R] = -1;
        }

        return er_code;
    }

    void reader_worker()
    {
        std::array<char, PIPE_SIZE> buffer{};

        while (true)
        {
#ifdef _WIN32
            const int bytes_read = _read(pipefd_[PIPE_R], buffer.data(),
                static_cast<unsigned>(buffer.size()));
#else
            const ssize_t bytes_read = read(pipefd_[PIPE_R], buffer.data(),
                buffer.size());
#endif
            if (bytes_read > 0)
            {
                const std::scoped_lock<std::mutex> lock(data_mutex_);
                captured_data_.append(buffer.data(),
                    static_cast<std::string::size_type>(bytes_read));
            }
            else if (bytes_read == 0 || errno != EINTR)
            {
                break;
            }
        }
    }

    int fd_old_{-1};
    int target_fd_;
    std::array<int, 2> pipefd_ = {{ -1, -1 }};
    std::FILE *target_stream_;
    std::mutex data_mutex_;
    std::string captured_data_;
    std::thread reader_thread_;
    static constexpr int PIPE_R = 0;
    static constexpr int PIPE_W = 1;
    static constexpr unsigned PIPE_SIZE = 4096;
};

template <auto GetStream>
class BaseCapture
{
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

private:
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
};

using StdoutCapture = BaseCapture<[] { return stdout; }>;
using StderrCapture = BaseCapture<[] { return stderr; }>;

#endif // CAPTURE_HPP
