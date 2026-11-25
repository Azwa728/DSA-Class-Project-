#pragma once
// ---------------------------------------------------------------
// DynamicArray<T>  -  a manually implemented resizable array.
// Used for: customers, products, transactions, graph adjacency lists.
//
// Key idea: keep a raw array (data) with a capacity. When it is full,
// allocate an array of DOUBLE the size, copy everything, free the old one.
// push_back is therefore O(1) amortized; insertAt / removeAt are O(n)
// because elements must be shifted.
// ---------------------------------------------------------------
#include <stdexcept>

template <typename T>
class DynamicArray {
    T* data;
    int count;
    int cap;

    void grow() {
        int newCap = cap * 2;
        T* bigger = new T[newCap];
        for (int i = 0; i < count; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        cap = newCap;
    }

public:
    explicit DynamicArray(int initialCapacity = 4)
        : data(nullptr), count(0), cap(initialCapacity < 1 ? 1 : initialCapacity) {
        data = new T[cap];
    }
    ~DynamicArray() { delete[] data; }

    // Rule of three: without these, copying an array would make two
    // objects share one buffer -> double delete.
    DynamicArray(const DynamicArray& o) : data(new T[o.cap]), count(o.count), cap(o.cap) {
        for (int i = 0; i < count; i++) data[i] = o.data[i];
    }
    DynamicArray& operator=(const DynamicArray& o) {
        if (this == &o) return *this;
        delete[] data;
        cap = o.cap;
        count = o.count;
        data = new T[cap];
        for (int i = 0; i < count; i++) data[i] = o.data[i];
        return *this;
    }

    void push_back(const T& item) {
        if (count == cap) grow();
        data[count++] = item;
    }

    void insertAt(int index, const T& item) {
        if (index < 0 || index > count) throw std::out_of_range("insertAt: bad index");
        if (count == cap) grow();
        for (int i = count; i > index; i--) data[i] = data[i - 1];
        data[index] = item;
        count++;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("removeAt: bad index");
        for (int i = index; i < count - 1; i++) data[i] = data[i + 1];
        count--;
    }

    void pop_back() {
        if (count == 0) throw std::out_of_range("pop_back: array is empty");
        count--;
    }

    void clear() { count = 0; }

    T& operator[](int i) {
        if (i < 0 || i >= count) throw std::out_of_range("index out of range");
        return data[i];
    }
    const T& operator[](int i) const {
        if (i < 0 || i >= count) throw std::out_of_range("index out of range");
        return data[i];
    }

    int size() const { return count; }
    int capacity() const { return cap; }
    bool isEmpty() const { return count == 0; }
};
