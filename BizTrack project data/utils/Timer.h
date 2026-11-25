#pragma once
#include <chrono>

// Simple stopwatch for the benchmarking demos.
class Timer {
    std::chrono::steady_clock::time_point start;
public:
    Timer() : start(std::chrono::steady_clock::now()) {}
    double ms() const {
        std::chrono::duration<double, std::milli> d = std::chrono::steady_clock::now() - start;
        return d.count();
    }
};
