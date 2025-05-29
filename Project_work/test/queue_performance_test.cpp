#include <iostream>
#include <iomanip>

#include <queue.hpp>

using namespace pc_queue;
using namespace std::chrono;

auto formatDuration(const nanoseconds ns) -> std::string
{
    auto ms = duration_cast<milliseconds>(ns);
    auto s = duration_cast<seconds>(ns);
    auto us = duration_cast<microseconds>(ns);

    if (s.count() > 0)
    {
        return std::to_string(s.count()) + "." + std::to_string((ms.count() % 1000) / 100)
            + " s";
    }

    if (ms.count() > 0)
    {
        return std::to_string(ms.count()) + "." +
            std::to_string((us.count() % 1000) / 100) + " ms";
    }

    if (us.count() > 0)
    {
        return std::to_string(us.count()) + "." +
            std::to_string((ns.count() % 1000) / 100) + " µs";
    }

    return std::to_string(ns.count()) + " ns";
}

void testSingleProducerSingleConsumer(
    const int numItems,
    const int queueSize)
{
    bool consumerReady(false);
    bool producerDone(false);
    double itemsPerSecond;
    Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, queueSize);
    std::atomic<int> consumed(0);

    std::cout << "=== Performance Test: One Producer, One Consumer ===" << std::endl;
    std::cout << "Number of elements: " << numItems << ", queue size: " << queueSize
        << std::endl;

    auto start = high_resolution_clock::now();
    std::thread consumer([&]()
    {
        consumerReady = true;
        while (  !producerDone
              || !queue.empty())
        {
            auto item = queue.pop(100);

            if (item.has_value())
            {
                consumed++;
            }
        }
    });

    while (!consumerReady)
    {
        std::this_thread::yield();
    }

    for (auto i = 0; i < numItems; ++i)
    {
        queue.push(i);
    }

    producerDone = true;
    consumer.join();
    auto end = high_resolution_clock::now();

    auto duration = end - start;
    itemsPerSecond = static_cast<double>(numItems)
        / (duration_cast<milliseconds>(duration).count() / 1000.0);

    std::cout << "Lead time: " << formatDuration(duration) << std::endl;
    std::cout << "Elements produced: " << numItems << std::endl;
    std::cout << "Elements consumed: " << consumed << std::endl;
    std::cout << "Performance: " << std::fixed << std::setprecision(2) << itemsPerSecond
        << " elements/sec" << std::endl;
    std::cout << std::endl;
}

void testMultiProducerMultiConsumer(
    const int numItems,
    const int queueSize,
    const int numProducers,
    const int numConsumers)
{
    bool producersDone(false);
    const int itemsPerProducer = numItems / numProducers;
    double itemsPerSecond;
    Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, queueSize);
    std::atomic<int> produced(0);
    std::atomic<int> consumed(0);
    std::vector<std::thread> consumers;
    std::vector<std::thread> producers;

    std::cout << "=== Performance Test: Many Producers, Many Consumers ===" << std::endl;
    std::cout << "Number of elements: " << numItems << ", queue size: " << queueSize
        << std::endl;
    std::cout << "Number of producers: " << numProducers << ", Number of consumers: "
        << numConsumers << std::endl;

    auto start = high_resolution_clock::now();

    consumers.reserve(numConsumers);

    for (auto i = 0; i < numConsumers; ++i)
    {
        consumers.emplace_back([&]()
        {
            while (  !producersDone
                  || !queue.empty())
            {
                auto item = queue.pop(10);

                if (item.has_value())
                {
                    consumed++;
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
                produced++;
            }
        });
    }

    for (auto& producer : producers)
    {
        producer.join();
    }

    producersDone = true;

    for (auto& consumer : consumers)
    {
        consumer.join();
    }

    auto end = high_resolution_clock::now();

    auto duration = end - start;
    itemsPerSecond = static_cast<double>(produced)
        / (duration_cast<milliseconds>(duration).count() / 1000.0);

    std::cout << "Lead time: " << formatDuration(duration) << std::endl;
    std::cout << "Elements produced: " << produced << std::endl;
    std::cout << "Elements consumed: " << consumed << std::endl;
    std::cout << "Performance: " << std::fixed << std::setprecision(2) << itemsPerSecond
        << " elements/sec" << std::endl;
    std::cout << std::endl;
}

void testPriorityQueue(
    const int numItems,
    const int numPriorities)
{
    double popPerSecond;
    double pushPerSecond;
    int consumed = 0;
    Queue<int, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

    std::cout << "=== Performance Test: Priority Queue ===" << std::endl;
    std::cout << "Number of elements: " << numItems << ", number of priorities: "
        << numPriorities << std::endl;


    auto startPush = high_resolution_clock::now();

    for (auto i = 0; i < numItems; ++i)
    {
        int priority = i % numPriorities;

        queue.push(i, priority, -1);
    }

    auto endPush = high_resolution_clock::now();

    auto startPop = high_resolution_clock::now();

    while (!queue.empty())
    {
        auto item = queue.pop();

        if (item.has_value())
        {
            consumed++;
        }
    }

    auto endPop = high_resolution_clock::now();

    auto durationPush = endPush - startPush;
    auto durationPop = endPop - startPop;
    auto durationTotal = durationPush + durationPop;

    pushPerSecond = static_cast<double>(numItems)
        / (duration_cast<milliseconds>(durationPush).count() / 1000.0);
    popPerSecond = static_cast<double>(consumed)
        / (duration_cast<milliseconds>(durationPop).count() / 1000.0);

    std::cout << "Add time: " << formatDuration(durationPush) << std::endl;
    std::cout << "Extraction time: " << formatDuration(durationPop) << std::endl;
    std::cout << "Total time: " << formatDuration(durationTotal) << std::endl;
    std::cout << "Add performance: " << std::fixed << std::setprecision(2)
        << pushPerSecond << " elements/sec" << std::endl;
    std::cout << "Extraction performance: " << std::fixed << std::setprecision(2)
        << popPerSecond << " elements/sec" << std::endl;
    std::cout << std::endl;
}

void compareQueueTypes(const int numItems)
{
    double priorityPopPerSecond;
    double priorityPushPerSecond;
    double regularPopPerSecond;
    double regularPushPerSecond;
    int priorityConsumed = 0;
    int regularConsumed = 0;
    Queue<int> regularQueue;
    Queue<int, int> priorityQueue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

    std::cout << "=== Performance Comparison: Regular Queue vs Priority Queue ==="
        << std::endl;
    std::cout << "Number of elements: " << numItems << std::endl;


    auto startRegularPush = high_resolution_clock::now();

    for (auto i = 0; i < numItems; ++i)
    {
        regularQueue.push(i);
    }

    auto endRegularPush = high_resolution_clock::now();

    auto startRegularPop = high_resolution_clock::now();

    while (!regularQueue.empty())
    {
        auto item = regularQueue.pop();
        if (item.has_value())
        {
            regularConsumed++;
        }
    }
    auto endRegularPop = high_resolution_clock::now();

    auto durationRegularPush = endRegularPush - startRegularPush;
    auto durationRegularPop = endRegularPop - startRegularPop;

    auto startPriorityPush = high_resolution_clock::now();

    for (auto i = 0; i < numItems; ++i)
    {
        priorityQueue.push(i, i % 10, -1);
    }

    auto endPriorityPush = high_resolution_clock::now();

    auto startPriorityPop = high_resolution_clock::now();

    while (!priorityQueue.empty())
    {
        auto item = priorityQueue.pop();

        if (item.has_value())
        {
            priorityConsumed++;
        }
    }

    auto endPriorityPop = high_resolution_clock::now();

    auto durationPriorityPush = endPriorityPush - startPriorityPush;
    auto durationPriorityPop = endPriorityPop - startPriorityPop;

    std::cout << "Regular queue:" << std::endl;
    std::cout << "  Add time: " << formatDuration(durationRegularPush) << std::endl;
    std::cout << "  Extraction time: " << formatDuration(durationRegularPop) << std::endl;
    std::cout << "  Total time: "
        << formatDuration(durationRegularPush + durationRegularPop) << std::endl;

    std::cout << "Priority Queue:" << std::endl;
    std::cout << "  Add time: " << formatDuration(durationPriorityPush) << std::endl;
    std::cout << "  Extraction time: " << formatDuration(durationPriorityPop)
        << std::endl;
    std::cout << "  Total time: "
        << formatDuration(durationPriorityPush + durationPriorityPop) << std::endl;

    regularPushPerSecond = static_cast<double>(numItems)
        / (duration_cast<milliseconds>(durationRegularPush).count() / 1000.0);
    regularPopPerSecond = static_cast<double>(regularConsumed)
        / (duration_cast<milliseconds>(durationRegularPop).count() / 1000.0);
    priorityPushPerSecond = static_cast<double>(numItems)
        / (duration_cast<milliseconds>(durationPriorityPush).count() / 1000.0);
    priorityPopPerSecond = static_cast<double>(priorityConsumed)
        / (duration_cast<milliseconds>(durationPriorityPop).count() / 1000.0);

    std::cout << "Comparison of append performance: " << std::endl;
    std::cout << "  Regular queue: " << std::fixed << std::setprecision(2)
        << regularPushPerSecond << " elements/sec" << std::endl;
    std::cout << "  Priority Queue: " << std::fixed << std::setprecision(2)
        << priorityPushPerSecond << " elements/sec" << std::endl;
    std::cout << "  Ratio: " << std::fixed << std::setprecision(2)
        << regularPushPerSecond / priorityPushPerSecond << "x" << std::endl;

    std::cout << "Comparison of extraction performance: " << std::endl;
    std::cout << "  Regular queue: " << std::fixed << std::setprecision(2)
        << regularPopPerSecond << " elements/sec" << std::endl;
    std::cout << "  Priority Queue: " << std::fixed << std::setprecision(2)
        << priorityPopPerSecond << " elements/sec" << std::endl;
    std::cout << "  Ratio: " << std::fixed << std::setprecision(2)
        << regularPopPerSecond / priorityPopPerSecond << "x" << std::endl;

    std::cout << std::endl;
}

auto main() -> int
{
    const int largeNumItems = 1000000;
    const int largeQueueSize = 10000;
    const int smallNumItems = 100000;
    const int smallQueueSize = 100;

    testSingleProducerSingleConsumer(smallNumItems, smallQueueSize);
    testSingleProducerSingleConsumer(largeNumItems, largeQueueSize);

    testMultiProducerMultiConsumer(smallNumItems, smallQueueSize, 2, 2);
    testMultiProducerMultiConsumer(largeNumItems, largeQueueSize, 4, 4);

    testPriorityQueue(smallNumItems, 5);

    compareQueueTypes(smallNumItems);

    return 0;
}
