#pragma once
// ---------------------------------------------------------------
// InventoryManager  -  products and stock, stored in a DynamicArray.
// Demonstrates: linear search, binary search (on a copy sorted by ID),
// and insertion sort / merge sort with timing.
// ---------------------------------------------------------------
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "models/Product.h"
#include "structures/DynamicArray.h"
#include "utils/Search.h"
#include "utils/Sorting.h"
#include "utils/Timer.h"
#include "utils/Console.h"

class InventoryManager {
    DynamicArray<Product> products;
    std::string filePath;

public:
    static constexpr int LOW_STOCK_LIMIT = 10;      // stock below this counts as "low"

    explicit InventoryManager(const std::string& path) : filePath(path) {}

    static void printHeaderRow() {
        std::cout << "  " << std::left << std::setw(8) << "ID" << std::setw(26) << "Name"
                  << std::setw(14) << "Category" << std::right << std::setw(14) << "Price"
                  << std::setw(8) << "Stock" << "\n";
        std::cout << "  " << std::string(70, '-') << "\n";
    }
    static void printRow(const Product& p) {
        std::cout << "  " << std::left << std::setw(8) << p.id << std::setw(26) << p.name
                  << std::setw(14) << p.category << std::right << std::setw(14) << Console::money(p.price)
                  << std::setw(8) << p.stock << "\n";
    }

    // ---------- file handling ----------
    bool load() {
        std::ifstream file(filePath);
        if (!file) { std::cout << "[warning] Could not open " << filePath << "\n"; return false; }
        std::string line;
        while (std::getline(file, line)) {
            Console::stripCR(line);
            if (line.empty()) continue;
            try {
                std::stringstream ss(line);
                Product p;
                std::string price, stock;
                std::getline(ss, p.id, '|');
                std::getline(ss, p.name, '|');
                std::getline(ss, p.category, '|');
                std::getline(ss, price, '|');
                std::getline(ss, stock, '|');
                p.price = std::stoi(price);
                p.stock = std::stoi(stock);
                products.push_back(p);
            } catch (...) {
                std::cout << "[warning] Skipped bad line in products file: " << line << "\n";
            }
        }
        return true;
    }

    void save() const {
        std::ofstream out(filePath);
        for (int i = 0; i < products.size(); i++) {
            const Product& p = products[i];
            out << p.id << "|" << p.name << "|" << p.category << "|" << p.price << "|" << p.stock << "\n";
        }
    }

    // ---------- accessors used by other modules ----------
    int count() const { return products.size(); }
    const DynamicArray<Product>& all() const { return products; }

    Product* find(const std::string& id) {                       // linear search by ID
        int i = linearSearch(products, [&](const Product& p) { return p.id == id; });
        return i < 0 ? nullptr : &products[i];
    }

    // ---------- menu actions ----------
    void viewAll() const {
        Console::header("ALL PRODUCTS");
        printHeaderRow();
        for (int i = 0; i < products.size(); i++) {
            printRow(products[i]);
            if ((i + 1) % Console::PAGE_SIZE == 0 && i + 1 < products.size() && !Console::continuePaging()) break;
        }
        std::cout << "\n  Total: " << products.size() << " products\n";
    }

    void searchById() {
        std::string id = Console::toUpper(Console::readText("  Product ID (e.g. P101): "));
        std::cout << "  1. Linear search\n  2. Binary search (sorts a copy by ID first)\n";
        int mode = Console::readIntInRange("  Method: ", 1, 2);
        long comparisons = 0;
        int pos;
        if (mode == 1) {
            pos = linearSearch(products, [&](const Product& p) { return p.id == id; }, &comparisons);
            std::cout << "\n  Linear search examined " << comparisons << " of " << products.size() << " products.\n\n";
            if (pos < 0) { std::cout << "  No product with ID " << id << ".\n"; return; }
            printHeaderRow();
            printRow(products[pos]);
        } else {
            DynamicArray<Product> sorted = products;                  // copy: original order untouched
            mergeSort(sorted, [](const Product& a, const Product& b) { return a.id < b.id; });
            pos = binarySearch(sorted, id, [](const Product& p) { return p.id; }, &comparisons);
            std::cout << "\n  Binary search examined " << comparisons << " of " << sorted.size() << " products.\n\n";
            if (pos < 0) { std::cout << "  No product with ID " << id << ".\n"; return; }
            printHeaderRow();
            printRow(sorted[pos]);
        }
    }

    void addInteractive() {
        std::string id = Console::toUpper(Console::readText("  New product ID (e.g. P151): "));
        if (find(id)) { std::cout << "  That ID already exists.\n"; return; }
        Product p;
        p.id = id;
        p.name = Console::readText("  Name: ");
        p.category = Console::readText("  Category: ");
        p.price = Console::readIntInRange("  Price (Rs.): ", 1, 100000000);
        p.stock = Console::readIntInRange("  Stock: ", 0, 1000000);
        products.push_back(p);
        save();
        std::cout << "  Product added.\n";
    }

    void updateInteractive() {
        std::string id = Console::toUpper(Console::readText("  Product ID to update: "));
        Product* p = find(id);
        if (!p) { std::cout << "  No product with ID " << id << ".\n"; return; }
        std::cout << "  Current: " << p->name << ", price " << Console::money(p->price) << ", stock " << p->stock << "\n";
        p->price = Console::readIntInRange("  New price (Rs.): ", 1, 100000000);
        p->stock = Console::readIntInRange("  New stock: ", 0, 1000000);
        save();
        std::cout << "  Product updated.\n";
    }

    void deleteInteractive() {
        std::string id = Console::toUpper(Console::readText("  Product ID to delete: "));
        int i = linearSearch(products, [&](const Product& p) { return p.id == id; });
        if (i < 0) { std::cout << "  No product with ID " << id << ".\n"; return; }
        products.removeAt(i);
        save();
        std::cout << "  Product deleted.\n";
    }

    void viewLowStock() const {
        Console::header("LOW-STOCK PRODUCTS");
        std::cout << "  (stock below " << LOW_STOCK_LIMIT << ")\n\n";
        printHeaderRow();
        int n = 0;
        for (int i = 0; i < products.size(); i++)
            if (products[i].stock < LOW_STOCK_LIMIT) { printRow(products[i]); n++; }
        if (n == 0) std::cout << "  None - all products are well stocked.\n";
        else std::cout << "\n  " << n << " product(s) need restocking.\n";
    }

    int lowStockCount() const {
        int n = 0;
        for (int i = 0; i < products.size(); i++) if (products[i].stock < LOW_STOCK_LIMIT) n++;
        return n;
    }

    void sortInteractive() const {
        Console::header("SORT PRODUCTS");
        std::cout << "    1. Price (low to high)\n    2. Price (high to low)\n"
                     "    3. Stock (low to high)\n    4. Name (A-Z)\n";
        int field = Console::readIntInRange("  Sort by: ", 1, 4);
        std::cout << "    1. Insertion sort\n    2. Merge sort\n";
        int algo = Console::readIntInRange("  Algorithm: ", 1, 2);

        DynamicArray<Product> copy = products;                        // never reorder the stored data
        auto lessFn = [field](const Product& a, const Product& b) {
            switch (field) {
                case 1: return a.price < b.price;
                case 2: return a.price > b.price;
                case 3: return a.stock < b.stock;
                default: return a.name < b.name;
            }
        };
        Timer t;
        if (algo == 1) insertionSort(copy, lessFn);
        else mergeSort(copy, lessFn);
        double ms = t.ms();

        std::cout << "\n";
        printHeaderRow();
        int limit = copy.size() < 15 ? copy.size() : 15;
        for (int i = 0; i < limit; i++) printRow(copy[i]);
        if (copy.size() > limit) std::cout << "  ... (" << copy.size() - limit << " more)\n";
        std::cout << "\n  " << (algo == 1 ? "Insertion" : "Merge") << " sort of " << copy.size()
                  << " products took " << ms << " ms.\n";
    }

    void menu() {
        while (true) {
            Console::header("PRODUCT & INVENTORY");
            std::cout << "    1. View all products\n    2. Search product by ID\n"
                         "    3. Add product\n    4. Update price / stock\n"
                         "    5. Delete product\n    6. View low-stock products\n"
                         "    7. Sort products\n\n    8. Back\n";
            int c = Console::readChoice();
            switch (c) {
                case 1: viewAll(); break;
                case 2: searchById(); break;
                case 3: addInteractive(); break;
                case 4: updateInteractive(); break;
                case 5: deleteInteractive(); break;
                case 6: viewLowStock(); break;
                case 7: sortInteractive(); break;
                case 8: return;
                default: std::cout << "  Invalid choice.\n"; continue;
            }
            Console::pause();
        }
    }
};
