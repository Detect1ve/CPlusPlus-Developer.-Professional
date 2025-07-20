#ifndef MY_LIST_HPP
#define MY_LIST_HPP

#include <gsl/gsl>

template <typename T, typename Alloc = std::allocator<T>>
class MyList
{
    struct Node // NOLINT(altera-struct-pack-align)
    {
        explicit Node(T const val): next(nullptr), val(val) { }

        [[nodiscard]] T getValue() const
        {
            return val;
        }

        [[nodiscard]] Node* getNext() const
        {
            return next;
        }

        void setNext(Node* const newNext)
        {
            next = newNext;
        }

    private:
        Node *next;
        T val;
    };

public:
    class iterator
    {
    public:
        explicit iterator(Node* const ptr) : current(ptr) {}

        iterator& operator++()
        {
            if (current)
            {
                current = current->getNext();
            }

            return *this;
        }

        T operator*() const
        {
            return current->getValue();
        }

        bool operator!=(const iterator& other) const
        {
            return current != other.current;
        }

    private:
        Node* current;
    };

    iterator begin()
    {
        return iterator(head);
    }

    iterator end()
    {
        return iterator(nullptr);
    }

    [[nodiscard]] bool empty() const
    {
        return head == nullptr;
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

        auto const newNode = static_cast<gsl::owner<Node*>>(nodeAllocator.allocate(1));

        new (newNode) Node(val);

        if (head == nullptr)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->setNext(newNode);
            tail = newNode;
        }

        size_++;
    }

    MyList() = default;
    MyList(const MyList&) = delete;
    MyList& operator=(const MyList&) = delete;
    MyList(MyList&&) = delete;
    MyList& operator=(MyList&&) = delete;

    ~MyList()
    {
        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        NodeAllocator nodeAllocator;

        Node* current = head;
        while (current)
        {
            Node* next = current->getNext();

            current->~Node();
            nodeAllocator.deallocate(current, 1);
            current = next;
        }
    }

private:
    Node *head = nullptr;
    Node *tail = nullptr;
    std::size_t size_ = 0;
};

#endif /* MY_LIST_HPP */
