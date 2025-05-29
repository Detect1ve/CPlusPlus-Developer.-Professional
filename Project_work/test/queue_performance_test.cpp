#if __cplusplus <= 201703L
#include <atomic>
#include <thread>
#endif
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <chrono>
#include <string>
#endif
#include <cmath>
#include <iomanip>
#include <iostream>

#include <queue.hpp>

using pc_queue::Queue;
using pc_queue::QueueMode;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

namespace
{
    constexpr float kMillisecondsInSecond = 1000.0;

    std::string formatDuration(const nanoseconds nanos)
    {
        constexpr int kMicrosecondsInMillisecond = 1000;
        constexpr int kHundred = 100;

        const auto micros = duration_cast<microseconds>(nanos);
        const auto millis = duration_cast<milliseconds>(nanos);
        const auto secs = duration_cast<seconds>(nanos);

        if (secs.count() > 0)
        {
            return std::to_string(secs.count()) + "."
                + std::to_string((millis.count() % kMicrosecondsInMillisecond) / kHundred)
                + " s";
        }

        if (millis.count() > 0)
        {
            return std::to_string(millis.count()) + "."
                + std::to_string((micros.count() % kMicrosecondsInMillisecond) / kHundred)
                + " ms";
        }

        if (micros.count() > 0)
        {
            return std::to_string(micros.count()) + "."
                + std::to_string((nanos.count() % kMicrosecondsInMillisecond) / kHundred)
                + " µs";
        }

        return std::to_string(nanos.count()) + " ns";
    }

    void testSingleProducerSingleConsumer(
        const int         numItems,
        const std::size_t queueSize)
    {
        bool consumerReady(false);
        bool producerDone(false);
        float itemsPerSecond = NAN;
        Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, queueSize);
        std::atomic<int> consumed(0);

        std::cout << "=== Performance Test: One Producer, One Consumer ===\n";
        std::cout << "Number of elements: " << numItems << ", queue size: " << queueSize
            << '\n';

        auto start = high_resolution_clock::now();
        std::thread consumer([&]()
        {
            consumerReady = true;
            while (  !producerDone
                || !queue.empty())
            {
                constexpr int kPopTimeoutMilliseconds = 100;
                const auto item = queue.pop(kPopTimeoutMilliseconds);

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
        itemsPerSecond = static_cast<float>(numItems)
            / (static_cast<float>(duration_cast<milliseconds>(duration).count())
            / kMillisecondsInSecond);

        std::cout << "Lead time: " << formatDuration(duration) << '\n';
        std::cout << "Elements produced: " << numItems << '\n';
        std::cout << "Elements consumed: " << consumed << '\n';
        std::cout << "Performance: " << std::fixed << std::setprecision(2)
            << itemsPerSecond << " elements/sec\n";
        std::cout << '\n';
    }

    void testMultiProducerMultiConsumer(
        const std::size_t numItems,
        const std::size_t queueSize,
        const std::size_t numProducers,
        const std::size_t numConsumers)
    {
        bool producersDone(false);
        const std::size_t itemsPerProducer = numItems / numProducers;
        float itemsPerSecond = NAN;
        Queue<std::size_t> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER,
            queueSize);
        std::atomic<int> produced(0);
        std::atomic<int> consumed(0);
        std::vector<std::thread> consumers;
        std::vector<std::thread> producers;

        std::cout << "=== Performance Test: Many Producers, Many Consumers ===\n";
        std::cout << "Number of elements: " << numItems << ", queue size: " << queueSize
            << '\n';
        std::cout << "Number of producers: " << numProducers << ", Number of consumers: "
            << numConsumers << '\n';

        auto start = high_resolution_clock::now();

        consumers.reserve(numConsumers);

        for (std::size_t i = 0; i < numConsumers; ++i)
        {
            consumers.emplace_back([&]()
            {
                while (  !producersDone
                    || !queue.empty())
                {
                    constexpr int kPopTimeoutMilliseconds = 10;
                    const auto item = queue.pop(kPopTimeoutMilliseconds);

                    if (item.has_value())
                    {
                        consumed++;
                    }
                }
            });
        }

        producers.reserve(numProducers);

        for (std::size_t i = 0; i < numProducers; ++i)
        {
            producers.emplace_back([&, i]()
            {
                for (std::size_t j = 0; j < itemsPerProducer; ++j)
                {
                    queue.push((i * itemsPerProducer) + j);
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
        itemsPerSecond = static_cast<float>(produced)
            / (static_cast<float>(duration_cast<milliseconds>(duration).count())
            / kMillisecondsInSecond);

        std::cout << "Lead time: " << formatDuration(duration) << '\n';
        std::cout << "Elements produced: " << produced << '\n';
        std::cout << "Elements consumed: " << consumed << '\n';
        std::cout << "Performance: " << std::fixed << std::setprecision(2)
            << itemsPerSecond << " elements/sec\n";
        std::cout << '\n';
    }

    void testPriorityQueue(
        const int numItems,
        const int numPriorities)
    {
        int consumed = 0;
        float popPerSecond = NAN;
        float pushPerSecond = NAN;
        Queue<int, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        std::cout << "=== Performance Test: Priority Queue ===\n";
        std::cout << "Number of elements: " << numItems << ", number of priorities: "
            << numPriorities << '\n';


        auto startPush = high_resolution_clock::now();

        for (auto i = 0; i < numItems; ++i)
        {
            const int priority = i % numPriorities;

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

        pushPerSecond = static_cast<float>(numItems)
            / (static_cast<float>(duration_cast<milliseconds>(durationPush).count())
            / kMillisecondsInSecond);
        popPerSecond = static_cast<float>(consumed)
            / (static_cast<float>(duration_cast<milliseconds>(durationPop).count())
            / kMillisecondsInSecond);

        std::cout << "Add time: " << formatDuration(durationPush) << '\n';
        std::cout << "Extraction time: " << formatDuration(durationPop) << '\n';
        std::cout << "Total time: " << formatDuration(durationTotal) << '\n';
        std::cout << "Add performance: " << std::fixed << std::setprecision(2)
            << pushPerSecond << " elements/sec\n";
        std::cout << "Extraction performance: " << std::fixed << std::setprecision(2)
            << popPerSecond << " elements/sec\n";
        std::cout << '\n';
    }

    void compareQueueTypes(const int numItems)
    {
        int priorityConsumed = 0;
        int regularConsumed = 0;
        float priorityPopPerSecond = NAN;
        float priorityPushPerSecond = NAN;
        float regularPopPerSecond = NAN;
        float regularPushPerSecond = NAN;
        Queue<int> regularQueue;
        Queue<int, int> priorityQueue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        std::cout << "=== Performance Comparison: Regular Queue vs Priority Queue ===\n";
        std::cout << "Number of elements: " << numItems << '\n';


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

        auto durRegularPush = endRegularPush - startRegularPush;
        auto durRegularPop = endRegularPop - startRegularPop;

        auto startPriorityPush = high_resolution_clock::now();

        for (auto i = 0; i < numItems; ++i)
        {
            constexpr int kNumPriorities = 10;
            priorityQueue.push(i, i % kNumPriorities, -1);
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

        auto durPriorityPush = endPriorityPush - startPriorityPush;
        auto durPriorityPop = endPriorityPop - startPriorityPop;

        std::cout << "Regular queue:\n";
        std::cout << "  Add time: " << formatDuration(durRegularPush) << '\n';
        std::cout << "  Extraction time: " << formatDuration(durRegularPop) << '\n';
        std::cout << "  Total time: " << formatDuration(durRegularPush + durRegularPop)
            << '\n';

        std::cout << "Priority Queue:\n";
        std::cout << "  Add time: " << formatDuration(durPriorityPush) << '\n';
        std::cout << "  Extraction time: " << formatDuration(durPriorityPop) << '\n';
        std::cout << "  Total time: " << formatDuration(durPriorityPush + durPriorityPop)
            << '\n';

        regularPushPerSecond = static_cast<float>(numItems)
            / (static_cast<float>(duration_cast<milliseconds>(durRegularPush).count())
            / kMillisecondsInSecond);
        regularPopPerSecond = static_cast<float>(regularConsumed)
            / (static_cast<float>(duration_cast<milliseconds>(durRegularPop).count())
            / kMillisecondsInSecond);
        priorityPushPerSecond = static_cast<float>(numItems)
            / (static_cast<float>(duration_cast<milliseconds>(durPriorityPush).count())
            / kMillisecondsInSecond);
        priorityPopPerSecond = static_cast<float>(priorityConsumed)
            / (static_cast<float>(duration_cast<milliseconds>(durPriorityPop).count())
            / kMillisecondsInSecond);

        std::cout << "Comparison of append performance:\n";
        std::cout << "  Regular queue: " << std::fixed << std::setprecision(2)
            << regularPushPerSecond << " elements/sec\n";
        std::cout << "  Priority Queue: " << std::fixed << std::setprecision(2)
            << priorityPushPerSecond << " elements/sec\n";
        std::cout << "  Ratio: " << std::fixed << std::setprecision(2)
            << regularPushPerSecond / priorityPushPerSecond << "x\n";

        std::cout << "Comparison of extraction performance:\n";
        std::cout << "  Regular queue: " << std::fixed << std::setprecision(2)
            << regularPopPerSecond << " elements/sec\n";
        std::cout << "  Priority Queue: " << std::fixed << std::setprecision(2)
            << priorityPopPerSecond << " elements/sec\n";
        std::cout << "  Ratio: " << std::fixed << std::setprecision(2)
            << regularPopPerSecond / priorityPopPerSecond << "x\n";

        std::cout << '\n';
    }
} // namespace

int main()
{
    try
    {
        constexpr int kNumPriorities = 5;
        const int largeNumItems = 1000000;
        const int smallNumItems = 100000;
        const std::size_t largeQueueSize = 10000;
        const std::size_t smallQueueSize = 100;

        testSingleProducerSingleConsumer(smallNumItems, smallQueueSize);
        testSingleProducerSingleConsumer(largeNumItems, largeQueueSize);

        testMultiProducerMultiConsumer(smallNumItems, smallQueueSize, 2, 2);
        testMultiProducerMultiConsumer(largeNumItems, largeQueueSize, 4, 4);

        testPriorityQueue(smallNumItems, kNumPriorities);

        compareQueueTypes(smallNumItems);

    }
    catch (const std::exception& e)
    {
        std::cerr << "An exception occurred: " << e.what() << '\n';

        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown exception occurred\n";

        return 1;
    }

    return 0;
}
