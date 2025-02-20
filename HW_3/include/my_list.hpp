#pragma once

template <typename T, typename Alloc = std::allocator<T>>
class MyList
{
    struct Node
    {
        Node(T const val): next(nullptr), val(val) { }

        [[nodiscard]] auto getValue() const -> T
        {
            return val;
        }

        [[nodiscard]] auto getNext() const -> Node*
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
        iterator(Node* const ptr) : current(ptr) {}

        auto operator++() -> iterator&
        {
            if (current)
            {
                current = current->getNext();
            }

            return *this;
        }

        auto operator*() const -> T
        {
            return current->getValue();
        }

        auto operator!=(const iterator& other) const -> bool
        {
            return current != other.current;
        }

    private:
        Node* current;
    };

    auto begin() -> iterator
    {
        return iterator(head);
    }

    auto end() -> iterator
    {
        return iterator(nullptr);
    }

    [[nodiscard]] auto empty() const -> bool
    {
        return head == nullptr;
    }

    [[nodiscard]] auto size() const -> std::size_t
    {
        return size_;
    }

    void push_back(const T& val)
    {
        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        NodeAllocator nodeAllocator;

        Node *const newNode = nodeAllocator.allocate(1);

        new (newNode) Node(val);

        if (!head)
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
