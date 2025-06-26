#include <future>
#include <thread>

#include <gtest/gtest.h>

#include <server.hpp>
#include <socket_wrapper.hpp>

class HW11 : public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::promise<std::uint16_t> port_promise;
        auto port_future = port_promise.get_future();

        server_thread_ = std::thread([&, &promise = port_promise]()
        {
            try
            {
                server_ = std::make_unique<Server>(promise, 0);
                server_->run();
            }
            catch (const std::exception&)
            {
                promise.set_exception(std::current_exception());
            }
        });

        port_ = port_future.get();
    }

    void TearDown() override
    {
        server_->stop();
        if (server_thread_.joinable())
        {
            server_thread_.join();
        }
    }

    [[nodiscard]] std::string communicate(const std::string& message) const 
    {
        const test_util::ClientSocket socket("127.0.0.1", port_);

        socket.send_data(message + "\n");

        return socket.receive_data();
    }

private:
    std::uint16_t port_ = 0;
    std::thread server_thread_;
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
