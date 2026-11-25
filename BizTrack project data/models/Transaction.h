#pragma once
#include <string>

// status is one of: PENDING, COMPLETED, FAILED
struct Transaction {
    std::string id;        // e.g. "T1001"
    int customerId = 0;
    std::string productId;
    int quantity = 0;
    long long amount = 0;  // price * quantity, in Rs.
    std::string status;
};
