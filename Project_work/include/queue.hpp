#pragma once

#include <condition_variable>
#if __GNUC__ < 15
#include <optional>
#endif
#include <queue>

namespace pc_queue
{
    /**
    * @brief Queue operating modes
    */
    enum class QueueMode
    {
        SINGLE_PRODUCER_SINGLE_CONSUMER,
        MULTI_PRODUCER_SINGLE_CONSUMER,
        SINGLE_PRODUCER_MULTI_CONSUMER,
        MULTI_PRODUCER_MULTI_CONSUMER
    };

    /**
    * @brief Thread-safe queue for Producer-Consumer workflow
    *
    * @tparam T            The type of data stored in the queue
    * @tparam PriorityType Priority type (default int)
    */
    template<typename T, typename PriorityType = int>
    class Queue
    {
    public:
        /**
        * @brief Structure for an element with priority
        */
        struct PriorityItem
        {
            /**
            * @brief Constructor
            *
            * @param[in] data_     Data
            * @param[in] priority_ Priority
            */
            PriorityItem(
                const T&           data_,
                const PriorityType priority_)
                :
                data(data_),
                priority(priority_) {}

            /**
            * @brief Get data
            *
            * @return Data
            */
            auto getData() const -> const T&
            {
                return data;
            }

            /**
            * @brief Comparison operator for sorting by priority
            * (lower priority = higher element)
            */
            auto operator<(const PriorityItem& other) const -> bool
            {
                return priority < other.priority;
            }
        private:
            T data;
            PriorityType priority;
        };

        /**
        * @brief Constructor
        *
        * @param[in] usePriority Should use priorities
        * @param[in] mode        Queue operating mode
        * @param[in] maxSize     Maximum queue size (0 - unlimited)
        */
        Queue(
            const bool      usePriority = false,
            const QueueMode mode = QueueMode::MULTI_PRODUCER_MULTI_CONSUMER,
            const size_t    maxSize = 0)
            :
            usePriority_(usePriority),
            closed_(false),
            mode_(mode),
            maxSize_(maxSize) {}

        /**
        * @brief Destructor
        */
        ~Queue()
        {
            close();
        }

        /**
        * @brief Places an element into a queue
        *
        * @param[in] item    Element to be placed in the queue
        * @param[in] timeout Wait timeout in milliseconds
        *                    (0 - no wait, -1 - infinite wait)
        *
        * @return true if the item was placed in the queue,
        *         false if a timeout occurred or the queue was closed
        */
        auto push(
            const T&  item,
            const int timeout = -1) -> bool
        {
            return pushImpl(item, PriorityType{}, timeout);
        }

        /**
        * @brief Places an element into a queue with the specified priority.
        *
        * @param[in] item     Element to be placed in the queue
        * @param[in] priority Element Priority
        * @param[in] timeout  Wait timeout in milliseconds
        *                     (0 - no wait, -1 - infinite wait)
        *
        * @return true if the item was placed in the queue,
        *         false if a timeout occurred or the queue was closed
        */
        auto push(
            const T&           item,
            const PriorityType priority,
            const int          timeout = -1) -> bool
        {
            return pushImpl(item, priority, timeout);
        }

        /**
        * @brief Removes an element from the queue
        *
        * @param[in] timeout Wait timeout in milliseconds
        *                    (0 - no wait, -1 - infinite wait)
        *
        * @return An element from the queue,
        *         or std::nullopt if the queue is empty or closed
        */
        auto pop(const int timeout = -1) -> std::optional<T>
        {
            bool success;
            std::unique_lock<std::mutex> lock(mutex_);
            T item;

            if (closed_ && empty())
            {
                return std::nullopt;
            }

            success = waitForNonEmpty(lock, timeout);
            if (!success)
            {
                return std::nullopt;
            }

            if (usePriority_)
            {
                item = priorityQueue_.top().getData();
                priorityQueue_.pop();
            }
            else
            {
                item = queue_.front();
                queue_.pop();
            }

            notFull_.notify_one();

            return item;
        }

        /**
        * @brief Checks if the queue is empty
        *
        * @return true if the queue is empty, false otherwise
        */
        auto empty() const -> bool
        {
            return (usePriority_ ? priorityQueue_.empty() : queue_.empty());
        }

        /**
        * @brief Returns the size of the queue
        *
        * @return Current queue size
        */
        auto size() const -> size_t
        {
            return (usePriority_ ? priorityQueue_.size() : queue_.size());
        }

        /**
        * @brief Closes the queue
        *
        * @details Once the queue is closed, new elements cannot be added, but existing
        * elements can be retrieved.
        */
        void close()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            closed_ = true;
            notEmpty_.notify_all();
            notFull_.notify_all();
        }

        /**
        * @brief Checks if the queue is closed
        *
        * @return true if the queue is closed, false otherwise
        */
        auto isClosed() const -> bool
        {
            std::lock_guard<std::mutex> lock(mutex_);

            return closed_;
        }

        /**
        * @brief Clears the queue
        */
        void clear()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            while (!queue_.empty())
            {
                queue_.pop();
            }

            if (usePriority_)
            {
                std::priority_queue<PriorityItem> empty;
                std::swap(priorityQueue_, empty);
            }

            notFull_.notify_all();
        }

    private:
        /**
        * @brief Implementing the push method
        *
        * @param[in] item     Element to be placed in the queue
        * @param[in] priority Element Priority
        * @param[in] timeout  Wait timeout in milliseconds
        *
        * @return true if the item was placed in the queue,
        *         false if a timeout occurred or the queue was closed
        */
        inline auto pushImpl(
            const T&           item,
            const PriorityType priority,
            const int          timeout) -> bool
        {
            bool isProducerActive = false;
            bool success = true;
            std::unique_lock<std::mutex> lock(mutex_);

            if (closed_)
            {
                return false;
            }

            if (  (  mode_ == QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER
                  || mode_ == QueueMode::SINGLE_PRODUCER_MULTI_CONSUMER)
               && producerActive_)
            {
                throw std::runtime_error("Only one producer allowed in this mode");
            }

            if (  mode_ == QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER
               || mode_ == QueueMode::SINGLE_PRODUCER_MULTI_CONSUMER)
            {
                producerActive_ = true;
                isProducerActive = true;
            }

            if (maxSize_ > 0)
            {
                size_t currentSize = size();

                if (currentSize >= maxSize_)
                {
                    success = waitForNotFull(lock, timeout);
                }
            }

            if (isProducerActive)
            {
                producerActive_ = false;
            }

            if (  !success
               || closed_)
            {
                return false;
            }

            if (usePriority_)
            {
                priorityQueue_.push(PriorityItem(item, priority));
            }
            else
            {
                queue_.push(item);
            }

            notEmpty_.notify_one();

            return true;
        }

        /**
        * @brief Waits until the queue is not empty or is closed.
        *
        * @param[in,out] lock    Mutex lock
        * @param[in]     timeout Wait timeout in milliseconds
        *
        * @return true if the queue is not empty,
        *         false if timeout or the queue is closed and empty
        */
        auto waitForNonEmpty(
            std::unique_lock<std::mutex>& lock,
            const int                     timeout) -> bool
        {
            bool isConsumerActive = false;
            bool queueEmpty;
            bool success = true;

            if (  (  mode_ == QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER
                  || mode_ == QueueMode::MULTI_PRODUCER_SINGLE_CONSUMER)
               && consumerActive_)
            {
                throw std::runtime_error("Only one consumer allowed in this mode");
            }

            if (  mode_ == QueueMode::SINGLE_PRODUCER_SINGLE_CONSUMER
               || mode_ == QueueMode::MULTI_PRODUCER_SINGLE_CONSUMER)
            {
                consumerActive_ = true;
                isConsumerActive = true;
            }

            queueEmpty = empty();

            if (  queueEmpty
               && !closed_)
            {
                if (timeout == 0)
                {
                    success = false;
                }
                else if (timeout > 0)
                {
                    auto predicate = [this]()
                    {
                        return !empty() || closed_;
                    };

                    success = notEmpty_.wait_for(lock, std::chrono::milliseconds(timeout),
                        predicate);
                }
                else
                {
                    notEmpty_.wait(lock, [this]()
                    {
                        return !empty() || closed_;
                    });
                }
            }

            if (isConsumerActive)
            {
                consumerActive_ = false;
            }

            if (empty())
            {
                return false;
            }

            return success;
        }

        /**
        * @brief Waits until a spot opens in the queue or the queue closes
        *
        * @param[in,out] lock    Mutex lock
        * @param[in]     timeout Wait timeout in milliseconds
        *
        * @return true if there is space in the queue, false if timeout or queue is closed
        */
        auto waitForNotFull(
            std::unique_lock<std::mutex>& lock,
            const int                     timeout) -> bool
        {
            size_t currentSize = size();
            bool queueFull = currentSize >= maxSize_;

            if (  queueFull
               && !closed_)
            {
                if (timeout == 0)
                {
                    return false;
                }

                if (timeout > 0)
                {
                    auto predicate = [this]()
                    {
                        return size() < maxSize_ || closed_;
                    };

                    return notFull_.wait_for(lock, std::chrono::milliseconds(timeout),
                        predicate);
                }

                notFull_.wait(lock, [this]()
                {
                    return size() < maxSize_ || closed_;
                });
            }

            return !closed_;
        }
        ///< Should use priorities
        bool usePriority_;
        ///< Queue close flag
        bool closed_;
        ///< Active Producer Flag
        bool producerActive_{false};
        ///< Active Consumer Flag
        bool consumerActive_{false};
        ///< Mutex for access synchronization
        mutable std::mutex mutex_;
        ///< Queue operating mode
        QueueMode mode_;
        ///< Maximum queue size (0 - unlimited)
        size_t maxSize_;
        ///< Condition variable for waiting for a non-empty queue
        std::condition_variable notEmpty_;
        ///< Condition variable for waiting for a non-full queue
        std::condition_variable notFull_;
        ///< Priority Queue
        std::priority_queue<PriorityItem> priorityQueue_;
        ///< Regular Queue
        std::queue<T> queue_;
    };
} // namespace pc_queue
