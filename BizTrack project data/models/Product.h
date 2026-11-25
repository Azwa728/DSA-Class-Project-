#pragma once
#include <string>

struct Product {
    std::string id;        // e.g. "P101"
    std::string name;
    std::string category;
    int price = 0;         // in Rs.
    int stock = 0;
};
