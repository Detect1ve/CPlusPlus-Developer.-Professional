#if __cplusplus <= 201703L
#include <thread>
#endif

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
    const int priority_high = 10;
    const int priority_medium = 5;
    Queue<std::string, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

    queue.push("Low", 1, -1);
    queue.push("High", priority_high, -1);
    queue.push("Medium", priority_medium, -1);

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
    bool done(false);
    const int items_to_produce = 100;
    const int max_queue_size = 10;
    const int pop_timeout_ms = 100;
    const int final_sleep_ms = 500;
    Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, max_queue_size);
    std::vector<int> consumed;
    std::mutex consumedMutex;

    std::thread consumer([&]()
    {
        while (  !done
              || !queue.empty())
        {
            auto item = queue.pop(pop_timeout_ms);

            if (item.has_value())
            {
                const std::lock_guard<std::mutex> lock(consumedMutex);

                consumed.emplace_back(item.value());
            }
        }
    });

    for (auto i = 0; i < items_to_produce; ++i)
    {
        ASSERT_TRUE(queue.push(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(final_sleep_ms));
    done = true;
    consumer.join();

    ASSERT_EQ(consumed.size(), items_to_produce);
    for (std::size_t i = 0; i < consumed.size(); ++i)
    {
        ASSERT_EQ(consumed[i], i);
    }
}

TEST(ProjectWork, MultiProducerMultiConsumer)
{
    bool done(false);
    const int itemsPerProducer = 1000;
    const int numConsumers = 4;
    const int numProducers = 4;
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);
    std::atomic<int> totalProduced(0);
    std::atomic<int> totalConsumed(0);
    std::vector<std::thread> consumers;
    std::vector<std::thread> producers;

    consumers.reserve(numConsumers);

    for (auto i = 0; i < numConsumers; ++i)
    {
        consumers.emplace_back([&]()
        {
            while (  !done
                  || !queue.empty())
            {
                const int pop_timeout_ms = 10;
                auto item = queue.pop(pop_timeout_ms);

                if (item.has_value())
                {
                    totalConsumed++;
                }
            }
        });
    }

    producers.reserve(numProducers);

    for (auto i = 0; i < numProducers; ++i)
    {
        producers.emplace_back([&, i]()
        {
            for (auto j = 0; j < itemsPerProducer; ++j)
            {
                queue.push((i * itemsPerProducer) + j);
                totalProduced++;
            }
        });
    }

    for (auto& producer : producers)
    {
        producer.join();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    done = true;
    for (auto& consumer : consumers)
    {
        consumer.join();
    }

    ASSERT_EQ(totalProduced.load(), numProducers * itemsPerProducer);
    ASSERT_EQ(totalConsumed.load(), totalProduced.load());
    ASSERT_TRUE(queue.empty());
}

TEST(ProjectWork, PopTimeout)
{
    Queue<int> queue;

    const int pop_timeout_ms = 100;
    auto start = std::chrono::steady_clock::now();
    auto item = queue.pop(pop_timeout_ms);
    auto end = std::chrono::steady_clock::now();

    ASSERT_FALSE(item.has_value());

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_GE(duration, pop_timeout_ms);
}

TEST(ProjectWork, PushTimeout)
{
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 1);

    ASSERT_TRUE(queue.push(1));

    const int push_timeout_ms = 100;
    auto start = std::chrono::steady_clock::now();
    auto result = queue.push(2, 0, push_timeout_ms);
    auto end = std::chrono::steady_clock::now();

    ASSERT_FALSE(result);

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_GE(duration, push_timeout_ms);
}
