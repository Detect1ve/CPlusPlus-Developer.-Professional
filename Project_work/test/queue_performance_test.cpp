#if __cplusplus <= 201703L
#include <atomic>
#include <thread>
#endif
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <chrono>
#include <string>
#endif
#include <cmath>
#include <cstddef> // std::size_t
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <vector> // std::vector

#include <custom_print.hpp>
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
        const std::size_t numItems,
        const std::size_t queueSize)
    {
        bool consumerReady(false);
        bool producerDone(false);
        float itemsPerSecond = NAN;
        Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, queueSize);
        std::atomic<int> consumed(0);

        cp::println("=== Performance Test: One Producer, One Consumer ===");
        cp::println("Number of elements: {}, queue size: {}", numItems, queueSize);

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

        cp::println("Lead time: {}", formatDuration(duration));
        cp::println("Elements produced: {}", numItems);
        cp::println("Elements consumed: {}", consumed.load(std::memory_order_relaxed));
        cp::println("Performance: {:.2f} elements/sec\n", itemsPerSecond);
    }

    struct TestConfig
    {
        std::size_t numItems;
        std::size_t queueSize;
        std::size_t numProducers;
        std::size_t numConsumers;
    };

    void testMultiProducerMultiConsumer(const TestConfig& config)
    {
        bool producersDone(false);
        const std::size_t itemsPerProducer = config.numItems / config.numProducers;
        float itemsPerSecond = NAN;
        Queue<std::size_t> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER,
            config.queueSize);
        std::atomic<int> produced(0);
        std::atomic<int> consumed(0);
        std::vector<std::thread> consumers;
        std::vector<std::thread> producers;

        cp::println("=== Performance Test: Many Producers, Many Consumers ===");
        cp::println("Number of elements: {}, queue size: {}", config.numItems,
            config.queueSize);
        cp::println("Number of producers: {}, Number of consumers: {}",
            config.numProducers, config.numConsumers);

        auto start = high_resolution_clock::now();

        consumers.reserve(config.numConsumers);

        for (std::size_t i = 0; i < config.numConsumers; i++)
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

        producers.reserve(config.numProducers);

        for (std::size_t i = 0; i < config.numProducers; i++)
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

        cp::println("Lead time: {}", formatDuration(duration));
        cp::println("Elements produced: {}", produced.load(std::memory_order_relaxed));
        cp::println("Elements consumed: {}", consumed.load(std::memory_order_relaxed));
        cp::println("Performance: {:.2f} elements/sec\n", itemsPerSecond);
    }

    void testPriorityQueue(
        const std::size_t numItems,
        const int         numPriorities)
    {
        int consumed = 0;
        float popPerSecond = NAN;
        float pushPerSecond = NAN;
        Queue<int, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        cp::println("=== Performance Test: Priority Queue ===");
        cp::println("Number of elements: {}, number of priorities: {}", numItems,
            numPriorities);

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

        cp::println("Add time: {}", formatDuration(durationPush));
        cp::println("Extraction time: {}", formatDuration(durationPop));
        cp::println("Total time: {}", formatDuration(durationTotal));
        cp::println("Add performance: {:.2f} elements/sec", pushPerSecond);
        cp::println("Extraction performance: {:.2f} elements/sec\n", popPerSecond);
    }

    void compareQueueTypes(const std::size_t numItems)
    {
        int priorityConsumed = 0;
        int regularConsumed = 0;
        float priorityPopPerSecond = NAN;
        float priorityPushPerSecond = NAN;
        float regularPopPerSecond = NAN;
        float regularPushPerSecond = NAN;
        Queue<int> regularQueue;
        Queue<int, int> priorityQueue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        cp::println("=== Performance Comparison: Regular Queue vs Priority Queue ===");
        cp::println("Number of elements: {}", numItems);

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

        cp::println("Regular queue:");
        cp::println("  Add time: {}", formatDuration(durRegularPush));
        cp::println("  Extraction time: {}", formatDuration(durRegularPop));
        cp::println("  Total time: {}", formatDuration(durRegularPush + durRegularPop));

        cp::println("Priority Queue:");
        cp::println("  Add time: {}", formatDuration(durPriorityPush));
        cp::println("  Extraction time: {}", formatDuration(durPriorityPop));
        cp::println("  Total time: {}", formatDuration(durPriorityPush + durPriorityPop));

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

        cp::println("Comparison of append performance:");
        cp::println("  Regular queue: {:.2f} elements/sec", regularPushPerSecond);
        cp::println("  Priority Queue: {:.2f} elements/sec", priorityPushPerSecond);
        cp::println("  Ratio: {:.2f}x", regularPushPerSecond / priorityPushPerSecond);

        cp::println("Comparison of extraction performance:");
        cp::println("  Regular queue: {:.2f} elements/sec", regularPopPerSecond);
        cp::println("  Priority Queue: {:.2f} elements/sec", priorityPopPerSecond);
        cp::println("  Ratio: {:.2f}x\n", regularPopPerSecond / priorityPopPerSecond);
    }
} // namespace

int main()
{
    try
    {
        const TestConfig large_config =
        {
            .numItems = 1000000,
            .queueSize = 10000,
            .numProducers = 4,
            .numConsumers = 4
        };
        const TestConfig small_config =
        {
            .numItems = 100000,
            .queueSize = 100,
            .numProducers = 2,
            .numConsumers = 2
        };
        constexpr int kNumPriorities = 5;

        testSingleProducerSingleConsumer(small_config.numItems, small_config.queueSize);
        testSingleProducerSingleConsumer(large_config.numItems, large_config.queueSize);

        testMultiProducerMultiConsumer(small_config);
        testMultiProducerMultiConsumer(large_config);

        testPriorityQueue(small_config.numItems, kNumPriorities);

        compareQueueTypes(small_config.numItems);

    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error(nullptr);

        return EXIT_FAILURE;
    }

    return 0;
}
