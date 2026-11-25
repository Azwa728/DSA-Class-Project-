#pragma once
// ---------------------------------------------------------------
// Stack<T>  -  LIFO stack built on top of DynamicArray.
// The top of the stack is the END of the array, so push and pop
// are O(1). Used in the Data Structure Lab (push/pop demonstration).
// ---------------------------------------------------------------
#include <stdexcept>
#include "structures/DynamicArray.h"

template <typename T>
class Stack {
    DynamicArray<T> items;

public:
    void push(const T& item) { items.push_back(item); }

    T pop() {
        if (items.isEmpty()) throw std::underflow_error("Stack is empty");
        T top = items[items.size() - 1];
        items.pop_back();
        return top;
    }

    const T& peek() const {
        if (items.isEmpty()) throw std::underflow_error("Stack is empty");
        return items[items.size() - 1];
    }

    // i = 0 is the BOTTOM of the stack.
    const T& at(int i) const { return items[i]; }

    bool isEmpty() const { return items.isEmpty(); }
    int size() const { return items.size(); }
};
