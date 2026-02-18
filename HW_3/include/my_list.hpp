#ifndef MY_LIST_HPP
#define MY_LIST_HPP

#include <gsl/gsl>

template <typename T, typename Alloc = std::allocator<T>>
class MyList
{
    class Node;
public:
    MyList() = default;
    ~MyList()
    {
        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        NodeAllocator nodeAllocator;

        Node* current_ = head_;
        while (current_)
        {
            Node* next = current_->getNext();

            current_->~Node();
            nodeAllocator.deallocate(current_, 1);
            current_ = next;
        }
    }
    MyList(const MyList&) = delete;
    MyList& operator=(const MyList&) = delete;
    MyList(MyList&&) = delete;
    MyList& operator=(MyList&&) = delete;

    class iterator
    {
    public:
        explicit iterator(Node* const ptr) : current_(ptr) {}

        iterator& operator++()
        {
            if (current_)
            {
                current_ = current_->getNext();
            }

            return *this;
        }

        T operator*() const
        {
            return current_->getValue();
        }

        bool operator!=(const iterator& other) const
        {
            return current_ != other.current_;
        }

    private:
        Node* current_;
    };

    iterator begin()
    {
        return iterator(head_);
    }

    iterator end()
    {
        return iterator(nullptr);
    }

    [[nodiscard]] bool empty() const
    {
        return head_ == nullptr;
    }

    [[nodiscard]] std::size_t size() const
    {
        return size_;
    }

    void push_back(const T& val)
    {
        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        NodeAllocator nodeAllocator;

        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        auto const newNode = static_cast<gsl::owner<Node*>>(nodeAllocator.allocate(1));

        new (newNode) Node(val);

        if (head_ == nullptr)
        {
            head_ = newNode;
            tail_ = newNode;
        }
        else
        {
            tail_->setNext(newNode);
            tail_ = newNode;
        }

        size_++;
    }

private:
    class Node
    {
    public:
        explicit Node(T const val): next_(nullptr), val_(val) {}

        [[nodiscard]] T getValue() const
        {
            return val_;
        }

        [[nodiscard]] Node* getNext() const
        {
            return next_;
        }

        void setNext(Node *const newNext)
        {
            next_ = newNext;
        }

    private:
        Node *next_;
        T val_;
    };

    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    std::size_t size_ = 0;
};

#endif // MY_LIST_HPP
