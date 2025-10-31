#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <gsl/pointers>

class StdoutCapture
{
public:
    StdoutCapture() : m_old_buf(std::cout.rdbuf())
    {
        std::cout.rdbuf(m_buffer.rdbuf());
    }

    ~StdoutCapture()
    {
        std::cout.rdbuf(m_old_buf);
    }

    StdoutCapture(const StdoutCapture&) = delete;
    StdoutCapture& operator=(const StdoutCapture&) = delete;
    StdoutCapture(StdoutCapture&&) = delete;
    StdoutCapture& operator=(StdoutCapture&&) = delete;

    static void Begin()
    {
        if (*instance() == nullptr)
        {
            *instance() = new StdoutCapture();
            (void)std::atexit(Destroy);
        }
    }

    static std::string End()
    {
        std::string output;
        if (*instance() != nullptr)
        {
            output = (*instance())->m_buffer.str();
            Destroy();
        }

        return output;
    }

private:
    static void Destroy()
    {
        gsl::owner<StdoutCapture*> to_delete = *instance();
        if (to_delete != nullptr)
        {
            *instance() = nullptr;
            delete to_delete;
        }
    }

    static gsl::owner<StdoutCapture*>* instance()
    {
        static gsl::owner<StdoutCapture*> s_instance = nullptr;

        return &s_instance;
    }

    std::streambuf* m_old_buf;
    std::stringstream m_buffer;
};

class StderrCapture
{
public:
    StderrCapture() : m_old_buf(std::cerr.rdbuf())
    {
        std::cerr.rdbuf(m_buffer.rdbuf());
    }

    ~StderrCapture()
    {
        std::cerr.rdbuf(m_old_buf);
    }

    StderrCapture(const StderrCapture&) = delete;
    StderrCapture& operator=(const StderrCapture&) = delete;
    StderrCapture(StderrCapture&&) = delete;
    StderrCapture& operator=(StderrCapture&&) = delete;

    static void Begin()
    {
        if (*instance() == nullptr)
        {
            *instance() = new StderrCapture();
            (void)std::atexit(Destroy);
        }
    }

    static std::string End()
    {
        std::string output;
        if (*instance() != nullptr)
        {
            output = (*instance())->m_buffer.str();
            Destroy();
        }

        return output;
    }

private:
    static void Destroy()
    {
        gsl::owner<StderrCapture*> to_delete = *instance();
        if (to_delete != nullptr)
        {
            *instance() = nullptr;
            delete to_delete;
        }
    }

    static gsl::owner<StderrCapture*>* instance()
    {
        static gsl::owner<StderrCapture*> s_instance = nullptr;

        return &s_instance;
    }

    std::streambuf* m_old_buf;
    std::stringstream m_buffer;
};

#endif // CAPTURE_HPP
