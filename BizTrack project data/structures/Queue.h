#pragma once
// ---------------------------------------------------------------
// Queue<T>  -  FIFO queue implemented as a CIRCULAR ARRAY.
// Used for: pending transactions (processed in the order added)
//           and as the helper structure inside BFS.
//
// head = index of the front element. The rear is (head + count) % cap,
// so enqueue/dequeue are O(1) and nothing is ever shifted.
// ---------------------------------------------------------------
#include <stdexcept>

template <typename T>
class Queue {
    T* data;
    int head;
    int count;
    int cap;

    void grow() {
        int newCap = cap * 2;
        T* bigger = new T[newCap];
        for (int i = 0; i < count; i++) bigger[i] = data[(head + i) % cap];
        delete[] data;
        data = bigger;
        head = 0;
        cap = newCap;
    }

public:
    explicit Queue(int initialCapacity = 8)
        : data(new T[initialCapacity < 1 ? 1 : initialCapacity]), head(0), count(0),
          cap(initialCapacity < 1 ? 1 : initialCapacity) {}
    ~Queue() { delete[] data; }

    Queue(const Queue&) = delete;             // not needed, so forbid (avoids double delete)
    Queue& operator=(const Queue&) = delete;

    void enqueue(const T& item) {
        if (count == cap) grow();
        data[(head + count) % cap] = item;
        count++;
    }

    T dequeue() {
        if (count == 0) throw std::underflow_error("Queue is empty");
        T item = data[head];
        head = (head + 1) % cap;
        count--;
        return item;
    }

    const T& front() const {
        if (count == 0) throw std::underflow_error("Queue is empty");
        return data[head];
    }

    // i = 0 is the front. Used to display the queue without removing anything.
    const T& at(int i) const {
        if (i < 0 || i >= count) throw std::out_of_range("Queue::at");
        return data[(head + i) % cap];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    void clear() { head = 0; count = 0; }
};
