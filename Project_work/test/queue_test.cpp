#include <queue.hpp>

#include <gtest/gtest.h>

using namespace pc_queue;

TEST(QueueTest, BasicFunctionality)
{
    Queue<int> queue;

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_TRUE(queue.push(3));

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 3);

    auto item1 = queue.pop();
    EXPECT_TRUE(item1.has_value());
    EXPECT_EQ(item1.value(), 1);

    auto item2 = queue.pop();
    EXPECT_TRUE(item2.has_value());
    EXPECT_EQ(item2.value(), 2);

    auto item3 = queue.pop();
    EXPECT_TRUE(item3.has_value());
    EXPECT_EQ(item3.value(), 3);

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    auto emptyItem = queue.pop(0);
    EXPECT_FALSE(emptyItem.has_value());
}

TEST(QueueTest, MaxSizeLimit)
{
    Queue<int> queue(2);

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));

    EXPECT_FALSE(queue.push(3, 0, 0));

    auto item = queue.pop();
    EXPECT_TRUE(item.has_value());
    EXPECT_EQ(item.value(), 1);

    EXPECT_TRUE(queue.push(3));
    EXPECT_EQ(queue.size(), 2);
}

TEST(QueueTest, CloseQueue)
{
    Queue<int> queue;

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));

    queue.close();
    EXPECT_TRUE(queue.isClosed());

    EXPECT_FALSE(queue.push(3));

    auto item1 = queue.pop();
    EXPECT_TRUE(item1.has_value());
    EXPECT_EQ(item1.value(), 1);

    auto item2 = queue.pop();
    EXPECT_TRUE(item2.has_value());
    EXPECT_EQ(item2.value(), 2);

    auto emptyItem = queue.pop();
    EXPECT_FALSE(emptyItem.has_value());
}

TEST(QueueTest, ClearQueue)
{
    Queue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(queue.size(), 3);

    queue.clear();

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

TEST(QueueTest, PriorityQueue)
{
    Queue<std::string, int> queue(0, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, true);

    queue.push("Low", 1, -1);
    queue.push("High", 10, -1);
    queue.push("Medium", 5, -1);

    auto item1 = queue.pop();
    EXPECT_TRUE(item1.has_value());
    EXPECT_EQ(item1.value(), "High");

    auto item2 = queue.pop();
    EXPECT_TRUE(item2.has_value());
    EXPECT_EQ(item2.value(), "Medium");

    auto item3 = queue.pop();
    EXPECT_TRUE(item3.has_value());
    EXPECT_EQ(item3.value(), "Low");
}

TEST(QueueTest, SingleProducerSingleConsumer)
{
    bool done(false);
    Queue<int> queue(10, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER);
    std::vector<int> consumed;
    std::mutex consumedMutex;

    std::thread consumer([&]()
    {
        while (  !done
              || !queue.empty())
        {
            auto item = queue.pop(100);

            if (item.has_value())
            {
                std::lock_guard<std::mutex> lock(consumedMutex);

                consumed.emplace_back(item.value());
            }
        }
    });

    for (auto i = 0; i < 100; ++i)
    {
        EXPECT_TRUE(queue.push(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    done = true;
    consumer.join();

    EXPECT_EQ(consumed.size(), 100);
    for (auto i = 0; i < consumed.size(); ++i)
    {
        EXPECT_EQ(consumed[i], i);
    }
}

TEST(QueueTest, MultiProducerMultiConsumer)
{
    bool done(false);
    const int itemsPerProducer = 1000;
    const int numConsumers = 4;
    const int numProducers = 4;
    Queue<int> queue(0, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER);
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
                auto item = queue.pop(10);

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
                queue.push(i * itemsPerProducer + j);
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

    EXPECT_EQ(totalProduced.load(), numProducers * itemsPerProducer);
    EXPECT_EQ(totalConsumed.load(), totalProduced.load());
    EXPECT_TRUE(queue.empty());
}

TEST(QueueTest, PopTimeout)
{
    Queue<int> queue;

    auto start = std::chrono::steady_clock::now();
    auto item = queue.pop(100);
    auto end = std::chrono::steady_clock::now();

    EXPECT_FALSE(item.has_value());

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_GE(duration, 90);
    EXPECT_LE(duration, 150);
}

TEST(QueueTest, PushTimeout)
{
    Queue<int> queue(1);

    EXPECT_TRUE(queue.push(1));

    auto start = std::chrono::steady_clock::now();
    auto result = queue.push(2, 0, 100);
    auto end = std::chrono::steady_clock::now();

    EXPECT_FALSE(result);

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_GE(duration, 90);
    EXPECT_LE(duration, 150);
}
