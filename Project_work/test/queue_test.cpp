#include <atomic> // std::atomic
#include <chrono> // std::chrono::milliseconds
#include <mutex> // std::mutex
#include <ranges>
#include <stop_token> // std::stop_token
#include <string> // std::string
#include <thread>
#include <vector> // std::vector

#include <gtest/gtest.h>

#include <queue.hpp>

using pc_queue::Queue;
using pc_queue::QueueMode;

TEST(ProjectWork, MaxSizeLimit)
{
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 2);

    ASSERT_TRUE(queue.push(1));
    ASSERT_TRUE(queue.push(2));

    ASSERT_FALSE(queue.push(3, 0, 0));

    auto item = queue.pop();
    if (item.has_value())
    {
        ASSERT_EQ(item.value(), 1);
    }
    else
    {
        FAIL() << "Expected item to have a value.";
    }

    ASSERT_TRUE(queue.push(3));
    ASSERT_EQ(queue.size(), 2);
}

TEST(ProjectWork, CloseQueue)
{
    Queue<int> queue;

    ASSERT_TRUE(queue.push(1));
    ASSERT_TRUE(queue.push(2));

    queue.close();
    ASSERT_TRUE(queue.isClosed());

    ASSERT_FALSE(queue.push(3));

    auto item1 = queue.pop();
    if (item1.has_value())
    {
        ASSERT_EQ(item1.value(), 1);
    }
    else
    {
        FAIL() << "Expected item1 to have a value.";
    }

    auto item2 = queue.pop();
    if (item2.has_value())
    {
        ASSERT_EQ(item2.value(), 2);
    }
    else
    {
        FAIL() << "Expected item2 to have a value.";
    }

    auto emptyItem = queue.pop();
    ASSERT_FALSE(emptyItem.has_value());
}

TEST(ProjectWork, ClearQueue)
{
    Queue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);

    ASSERT_EQ(queue.size(), 3);

    queue.clear();

    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.size(), 0);
}

TEST(ProjectWork, PriorityQueue)
{
    constexpr int PRIORITY_HIGH = 10;
    constexpr int PRIORITY_MEDIUM = 5;
    Queue<std::string, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

    queue.push("Low", 1, -1);
    queue.push("High", PRIORITY_HIGH, -1);
    queue.push("Medium", PRIORITY_MEDIUM, -1);

    auto item1 = queue.pop();
    if (item1.has_value())
    {
        ASSERT_EQ(item1.value(), "High");
    }
    else
    {
        FAIL() << "Expected item1 to have a value.";
    }

    auto item2 = queue.pop();
    if (item2.has_value())
    {
        ASSERT_EQ(item2.value(), "Medium");
    }
    else
    {
        FAIL() << "Expected item2 to have a value.";
    }

    auto item3 = queue.pop();
    if (item3.has_value())
    {
        ASSERT_EQ(item3.value(), "Low");
    }
    else
    {
        FAIL() << "Expected item3 to have a value.";
    }
}

TEST(ProjectWork, SingleProducerSingleConsumer)
{
    constexpr int ITEMS_TO_PRODUCE{100};
    constexpr int MAX_QUEUE_SIZE{10};
    constexpr int POP_TIMEOUT_MS{100};
    constexpr int FINAL_SLEEP_MS{500};
    Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, MAX_QUEUE_SIZE);
    std::vector<int> consumed;
    std::mutex consumedMutex;

    std::jthread consumer([&](const std::stop_token& stoken)
    {
        while (  !stoken.stop_requested()
              || !queue.empty())
        {
            auto item = queue.pop(POP_TIMEOUT_MS);

            if (item.has_value())
            {
                const std::scoped_lock<std::mutex> lock(consumedMutex);

                consumed.emplace_back(item.value());
            }
        }
    });

    for (int i = 0; i < ITEMS_TO_PRODUCE; i++)
    {
        ASSERT_TRUE(queue.push(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(FINAL_SLEEP_MS));
    consumer.request_stop();
    consumer.join();

    ASSERT_EQ(consumed.size(), ITEMS_TO_PRODUCE);
    for (const auto& [idx, value] : std::views::enumerate(consumed))
    {
        ASSERT_EQ(value, idx);
    }
}

TEST(ProjectWork, MultiProducerMultiConsumer)
{
    constexpr int ITEMS_PER_PRODUCER{1000};
    constexpr int NUM_CONSUMERS{4};
    constexpr int NUM_PRODUCERS{4};
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);
    std::atomic<int> totalProduced(0);
    std::atomic<int> totalConsumed(0);
    std::vector<std::jthread> consumers;
    std::vector<std::jthread> producers;

    consumers.reserve(NUM_CONSUMERS);

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        consumers.emplace_back([&](const std::stop_token& stoken)
        {
            while (  !stoken.stop_requested()
                  || !queue.empty())
            {
                constexpr int POP_TIMEOUT_MS{10};
                auto item = queue.pop(POP_TIMEOUT_MS);

                if (item.has_value())
                {
                    totalConsumed++;
                }
            }
        });
    }

    producers.reserve(NUM_PRODUCERS);

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        producers.emplace_back([&, i]()
        {
            for (int j = 0; j < ITEMS_PER_PRODUCER; j++)
            {
                queue.push((i * ITEMS_PER_PRODUCER) + j);
                totalProduced++;
            }
        });
    }

    producers.clear();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (auto& consumer : consumers)
    {
        consumer.request_stop();
    }

    consumers.clear();

    ASSERT_EQ(totalProduced.load(), NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    ASSERT_EQ(totalConsumed.load(), totalProduced.load());
    ASSERT_TRUE(queue.empty());
}

TEST(ProjectWork, PopTimeout)
{
    const int PUSH_TIMEOUT_MS{100};
    Queue<int> queue;

    auto start = std::chrono::steady_clock::now();
    auto item = queue.pop(PUSH_TIMEOUT_MS);
    auto end = std::chrono::steady_clock::now();

    ASSERT_FALSE(item.has_value());

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_GE(duration, PUSH_TIMEOUT_MS);
}

TEST(ProjectWork, PushTimeout)
{
    constexpr int PUSH_TIMEOUT_MS{100};
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 1);

    ASSERT_TRUE(queue.push(1));

    auto start = std::chrono::steady_clock::now();
    auto result = queue.push(2, 0, PUSH_TIMEOUT_MS);
    auto end = std::chrono::steady_clock::now();

    ASSERT_FALSE(result);

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_GE(duration, PUSH_TIMEOUT_MS);
}
