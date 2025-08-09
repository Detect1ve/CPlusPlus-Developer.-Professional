#ifndef CAPTURE_HPP // NOLINT(llvm-header-guard)
#define CAPTURE_HPP

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
        if (!instance())
        {
            instance() = std::make_unique<StdoutCapture>();
        }
    }

    static std::string End()
    {
        std::string output;
        if (instance())
        {
            output = instance()->m_buffer.str();
            instance().reset();
        }

        return output;
    }

private:
    static std::unique_ptr<StdoutCapture>& instance()
    {
        static std::unique_ptr<StdoutCapture> s_instance;

        return s_instance;
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
        if (!instance())
        {
            instance() = std::make_unique<StderrCapture>();
        }
    }

    static std::string End()
    {
        std::string output;
        if (instance())
        {
            output = instance()->m_buffer.str();
            instance().reset();
        }
        return output;
    }

private:
    static std::unique_ptr<StderrCapture>& instance()
    {
        static std::unique_ptr<StderrCapture> s_instance;

        return s_instance;
    }

    std::streambuf* m_old_buf;
    std::stringstream m_buffer;
};

#endif // CAPTURE_HPP
