#ifndef QUEUE_H
#define QUEUE_H

#include <memory_resource>
#include <iterator>
#include <memory>

template<typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        
        template<typename... Args>
        Node(Args&&... args) 
            : data(std::forward<Args>(args)...), next(nullptr) {
        }
    };

    Node* front_node;
    Node* back_node;
    size_t queue_size;
    std::pmr::polymorphic_allocator<Node> allocator;

public:
    // конструкторы
    explicit Queue(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : front_node(nullptr), back_node(nullptr), queue_size(0), allocator(mr) {}

    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;

    Queue(Queue&& other) noexcept 
        : front_node(other.front_node), back_node(other.back_node), 
          queue_size(other.queue_size), allocator(other.allocator) {
        other.front_node = nullptr;
        other.back_node = nullptr;
        other.queue_size = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            clear();
            front_node = other.front_node;
            back_node = other.back_node;
            queue_size = other.queue_size;
            allocator = other.allocator;
            other.front_node = nullptr;
            other.back_node = nullptr;
            other.queue_size = 0;
        }
        return *this;
    }

    ~Queue() {
        clear();
    }

    // основные методы очереди
    template<typename U>
    void push(U&& value) {
        Node* new_node = allocator.allocate(1);
        try {
            allocator.construct(new_node, std::forward<U>(value));
        } catch (...) {
            allocator.deallocate(new_node, 1);
            throw;
        }

        if (!back_node) {
            front_node = back_node = new_node;
        } else {
            back_node->next = new_node;
            back_node = new_node;
        }
        queue_size++;
    }

    void pop() {
        if (front_node) {
            Node* temp = front_node;
            front_node = front_node->next;
            if (!front_node) {
                back_node = nullptr;
            }
            allocator.destroy(temp);
            allocator.deallocate(temp, 1);
            queue_size--;
        }
    }

    T& front() { return front_node->data; }
    const T& front() const { return front_node->data; }
    
    T& back() { return back_node->data; }
    const T& back() const { return back_node->data; }

    bool empty() const { return queue_size == 0; }
    size_t size() const { return queue_size; }

    void clear() {
        while (!empty()) {
            pop();
        }
    }

    // итератор
    class Iterator {
    private:
        Node* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node = nullptr) : current(node) {}

        reference operator*() const { return current->data; }
        pointer operator->() const { return &current->data; }

        Iterator& operator++() {
            if (current) {
                current = current->next;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() { return Iterator(front_node); }
    Iterator end() { return Iterator(nullptr); }
};

#endif