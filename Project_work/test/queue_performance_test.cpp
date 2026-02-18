#include <atomic> // std::atomic
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <chrono>
#include <string>
#endif
#include <cstddef> // std::size_t
#include <cstdint> // std::uint64_t
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <limits> // std::numeric_limits
#include <stop_token> // std::stop_token
#include <thread>
#include <utility>
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
    constexpr double MILLISEC_IN_SEC = 1000.0;

    std::string formatDuration(const nanoseconds nanos)
    {
        constexpr int HUNDRED = 100;
        constexpr int MICROSEC_IN_MILLISEC = 1000;

        const auto micros = duration_cast<microseconds>(nanos);
        const auto millis = duration_cast<milliseconds>(nanos);
        const auto secs = duration_cast<seconds>(nanos);

        if (secs.count() > 0)
        {
            return std::to_string(secs.count()) + "."
                + std::to_string((millis.count() % MICROSEC_IN_MILLISEC) / HUNDRED)
                + " s";
        }

        if (millis.count() > 0)
        {
            return std::to_string(millis.count()) + "."
                + std::to_string((micros.count() % MICROSEC_IN_MILLISEC) / HUNDRED)
                + " ms";
        }

        if (micros.count() > 0)
        {
            return std::to_string(micros.count()) + "."
                + std::to_string((nanos.count() % MICROSEC_IN_MILLISEC) / HUNDRED)
                + " µs";
        }

        return std::to_string(nanos.count()) + " ns";
    }

    void testSingleProducerSingleConsumer(
        const int         numItems,
        const std::size_t queueSize)
    {
        bool consumerReady(false);
        double itemsPerSecond = std::numeric_limits<double>::quiet_NaN();
        Queue<int> queue(false, QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER, queueSize);
        std::atomic<int> consumed(0);

        cp::println("=== Performance Test: One Producer, One Consumer ===");
        cp::println("Number of elements: {}, queue size: {}", numItems, queueSize);

        auto start = high_resolution_clock::now();
        std::jthread consumer([&](const std::stop_token& stoken)
        {
            consumerReady = true;
            while (  !stoken.stop_requested()
                  || !queue.empty())
            {
                constexpr int POP_TIMEOUT_MILLISEC = 100;
                const auto item = queue.pop(POP_TIMEOUT_MILLISEC);

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

        for (int i = 0; i < numItems; i++)
        {
            queue.push(i);
        }

        consumer.request_stop();
        consumer.join();
        auto end = high_resolution_clock::now();

        auto duration = end - start;
        itemsPerSecond = static_cast<double>(numItems)
            / (static_cast<double>(duration_cast<milliseconds>(duration).count())
            / MILLISEC_IN_SEC);

        cp::println("Lead time: {}", formatDuration(duration));
        cp::println("Elements produced: {}", numItems);
        cp::println("Elements consumed: {}", consumed.load(std::memory_order_relaxed));
        cp::println("Performance: {:.2f} elements/sec\n", itemsPerSecond);
    }

    struct TestConfig
    {
        int numItems;
        std::size_t queueSize;
        int numProducers;
        int numConsumers;
    };

    void testMultiProducerMultiConsumer(const TestConfig& config)
    {
        const int itemsPerProducer = config.numItems / config.numProducers;
        double itemsPerSecond = std::numeric_limits<double>::quiet_NaN();
        Queue<int> queue(false, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER,
            config.queueSize);
        std::atomic<int> produced(0);
        std::atomic<int> consumed(0);
        std::vector<std::jthread> consumers;
        std::vector<std::jthread> producers;

        cp::println("=== Performance Test: Many Producers, Many Consumers ===");
        cp::println("Number of elements: {}, queue size: {}", config.numItems,
            config.queueSize);
        cp::println("Number of producers: {}, Number of consumers: {}",
            config.numProducers, config.numConsumers);

        auto start = high_resolution_clock::now();

        consumers.reserve(static_cast<std::uint64_t>(config.numConsumers)); // FIXME

        for (int i = 0; i < config.numConsumers; i++)
        {
            consumers.emplace_back([&](const std::stop_token& stoken)
            {
                while (  !stoken.stop_requested()
                      || !queue.empty())
                {
                    constexpr int POP_TIMEOUT_MILLISEC = 10;
                    const auto item = queue.pop(POP_TIMEOUT_MILLISEC);

                    if (item.has_value())
                    {
                        consumed++;
                    }
                }
            });
        }

        producers.reserve(static_cast<std::uint64_t>(config.numProducers)); // FIXME

        for (int i = 0; i < config.numProducers; i++)
        {
            producers.emplace_back([&, i]()
            {
                for (int j = 0; j < itemsPerProducer; j++)
                {
                    queue.push((i * itemsPerProducer) + j);
                    produced++;
                }
            });
        }

        producers.clear();

        for (auto& consumer : consumers)
        {
            consumer.request_stop();
        }

        consumers.clear();

        auto end = high_resolution_clock::now();

        auto duration = end - start;
        itemsPerSecond = static_cast<double>(produced)
            / (static_cast<double>(duration_cast<milliseconds>(duration).count())
            / MILLISEC_IN_SEC);

        cp::println("Lead time: {}", formatDuration(duration));
        cp::println("Elements produced: {}", produced.load(std::memory_order_relaxed));
        cp::println("Elements consumed: {}", consumed.load(std::memory_order_relaxed));
        cp::println("Performance: {:.2f} elements/sec\n", itemsPerSecond);
    }

    void testPriorityQueue(
        const int numItems,
        const int numPriorities)
    {
        double popPerSecond = std::numeric_limits<double>::quiet_NaN();
        double pushPerSecond = std::numeric_limits<double>::quiet_NaN();
        int consumed = 0;
        Queue<int, int> queue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        cp::println("=== Performance Test: Priority Queue ===");
        cp::println("Number of elements: {}, number of priorities: {}", numItems,
            numPriorities);

        auto startPush = high_resolution_clock::now();

        for (int i = 0; i < numItems; i++)
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

        pushPerSecond = static_cast<double>(numItems)
            / (static_cast<double>(duration_cast<milliseconds>(durationPush).count())
            / MILLISEC_IN_SEC);
        popPerSecond = static_cast<double>(consumed)
            / (static_cast<double>(duration_cast<milliseconds>(durationPop).count())
            / MILLISEC_IN_SEC);

        cp::println("Add time: {}", formatDuration(durationPush));
        cp::println("Extraction time: {}", formatDuration(durationPop));
        cp::println("Total time: {}", formatDuration(durationTotal));
        cp::println("Add performance: {:.2f} elements/sec", pushPerSecond);
        cp::println("Extraction performance: {:.2f} elements/sec\n", popPerSecond);
    }

    void compareQueueTypes(const int numItems)
    {
        int priorityConsumed = 0;
        int regularConsumed = 0;
        double priorityPopPerSecond = std::numeric_limits<double>::quiet_NaN();
        double priorityPushPerSecond = std::numeric_limits<double>::quiet_NaN();
        double regularPopPerSecond = std::numeric_limits<double>::quiet_NaN();
        double regularPushPerSecond = std::numeric_limits<double>::quiet_NaN();
        Queue<int> regularQueue;
        Queue<int, int> priorityQueue(true, QueueMode::MULTI_PRODUCER_MULTI_CONSUMER, 0);

        cp::println("=== Performance Comparison: Regular Queue vs Priority Queue ===");
        cp::println("Number of elements: {}", numItems);

        auto startRegularPush = high_resolution_clock::now();

        for (int i = 0; std::cmp_less(i, numItems); i++)
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

        for (int i = 0; std::cmp_less(i, numItems); i++)
        {
            constexpr int NUM_PRIORITIES = 10;
            priorityQueue.push(i, i % NUM_PRIORITIES, -1);
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

        regularPushPerSecond = static_cast<double>(numItems)
            / (static_cast<double>(duration_cast<milliseconds>(durRegularPush).count())
            / MILLISEC_IN_SEC);
        regularPopPerSecond = static_cast<double>(regularConsumed)
            / (static_cast<double>(duration_cast<milliseconds>(durRegularPop).count())
            / MILLISEC_IN_SEC);
        priorityPushPerSecond = static_cast<double>(numItems)
            / (static_cast<double>(duration_cast<milliseconds>(durPriorityPush).count())
            / MILLISEC_IN_SEC);
        priorityPopPerSecond = static_cast<double>(priorityConsumed)
            / (static_cast<double>(duration_cast<milliseconds>(durPriorityPop).count())
            / MILLISEC_IN_SEC);

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
        constexpr int NUM_PRIORITIES = 5;

        testSingleProducerSingleConsumer(small_config.numItems, small_config.queueSize);
        testSingleProducerSingleConsumer(large_config.numItems, large_config.queueSize);

        testMultiProducerMultiConsumer(small_config);
        testMultiProducerMultiConsumer(large_config);

        testPriorityQueue(small_config.numItems, NUM_PRIORITIES);

        compareQueueTypes(small_config.numItems);

    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error();

        return EXIT_FAILURE;
    }

    return 0;
}
