#include <cstdint> // std::uint16_t
#include <exception> // std::exception
#include <future>
#include <memory> // std::unique_ptr
#include <string> // std::string
#include <thread> // std::thread

#include <gtest/gtest.h>

#include <server.hpp>
#include <socket_wrapper.hpp>

class HW11 : public ::testing::Test
{
public:
    HW11() = default;
    ~HW11() override = default;
    HW11(const HW11&) = delete;
    HW11(HW11&&) = delete;
    HW11& operator=(const HW11&) = delete;
    HW11& operator=(HW11&&) = delete;

protected:
    void SetUp() override
    {
        std::promise<std::uint16_t> port_promise;
        auto port_future = port_promise.get_future();

        server_ = std::make_unique<Server>(port_promise, std::int16_t{0});
        server_thread_ = std::thread([this]()
        {
            try
            {
                server_->run();
            }
            catch (const std::exception& e)
            {
                server_exception_ = std::make_exception_ptr(e);
            }
        });

        port_ = port_future.get();
    }

    void TearDown() override
    {
        if (server_)
        {
            server_->stop();
        }

        if (server_thread_.joinable())
        {
            server_thread_.join();
        }

        if (server_exception_)
        {
            try
            {
                std::rethrow_exception(server_exception_);
            }
            catch (const std::exception& e)
            {
                FAIL() << "Server thread threw exception: " << e.what();
            }
        }
    }

    [[nodiscard]] std::string communicate(const std::string& message) const
    {
        const test_util::ClientSocket socket("127.0.0.1", port_);

        socket.send_data(message + '\n');

        return socket.receive_data();
    }

private:
    std::exception_ptr server_exception_ = nullptr;
    std::thread server_thread_;
    std::uint16_t port_ = 0;
    std::unique_ptr<Server> server_;
};

TEST_F(HW11, InsertDuplicate)
{
    ASSERT_EQ(communicate("INSERT A 0 lean"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 0 understand"), "ERR duplicate 0\n");
}

TEST_F(HW11, UnknownCommand)
{
    ASSERT_EQ(communicate("UNKNOWN"), "ERR unknown command\n");
}

TEST_F(HW11, Truncate)
{
    const std::string expected_intersection =
        "0,lean,lean\n"
        "OK\n";

    ASSERT_EQ(communicate("INSERT A 0 lean"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 0 lean"), "OK\n");
    auto response = communicate("INTERSECTION");

    ASSERT_EQ(response, expected_intersection);

    ASSERT_EQ(communicate("TRUNCATE A"), "OK\n");
    ASSERT_EQ(communicate("TRUNCATE B"), "OK\n");
    response = communicate("INTERSECTION");
    ASSERT_EQ(response, "OK\n");
}

TEST_F(HW11, FromReadme)
{
    const std::string expected_intersection =
        "3,violation,proposal\n"
        "4,quality,example\n"
        "5,precision,lake\n"
        "OK\n";
    const std::string expected_symmetric_difference =
        "0,lean,\n"
        "1,sweater,\n"
        "2,frank,\n"
        "6,,flour\n"
        "7,,wonder\n"
        "8,,selection\n"
        "OK\n";

    ASSERT_EQ(communicate("INSERT A 0 lean"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 1 sweater"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 2 frank"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 3 violation"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 4 quality"), "OK\n");
    ASSERT_EQ(communicate("INSERT A 5 precision"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 3 proposal"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 4 example"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 5 lake"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 6 flour"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 7 wonder"), "OK\n");
    ASSERT_EQ(communicate("INSERT B 8 selection"), "OK\n");
    auto response = communicate("INTERSECTION");

    ASSERT_EQ(response, expected_intersection);

    response = communicate("SYMMETRIC_DIFFERENCE");
    ASSERT_EQ(response, expected_symmetric_difference);
}
