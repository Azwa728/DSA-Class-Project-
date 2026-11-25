#pragma once
// ---------------------------------------------------------------
// CustomerManager  -  customer records.
//   DynamicArray<Customer> : holds the records in file order (used for listing / saving)
//   BST<int, Customer>     : index by customer ID (used for fast lookup / sorted listing)
// Both are kept in sync on every add / update / delete.
// ---------------------------------------------------------------
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "models/Customer.h"
#include "structures/DynamicArray.h"
#include "structures/BST.h"
#include "utils/Search.h"
#include "utils/Console.h"

class CustomerManager {
    DynamicArray<Customer> customers;
    BST<int, Customer> index;
    std::string filePath;

    int arrayIndexOf(int id) const {
        return linearSearch(customers, [&](const Customer& c) { return c.id == id; });
    }

public:
    explicit CustomerManager(const std::string& path) : filePath(path) {}

    static void printHeaderRow() {
        std::cout << "  " << std::left << std::setw(8) << "ID" << std::setw(24) << "Name" << "City\n";
        std::cout << "  " << std::string(46, '-') << "\n";
    }
    static void printRow(const Customer& c) {
        std::cout << "  " << std::left << std::setw(8) << c.id << std::setw(24) << c.name << c.city << "\n";
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
                std::string idStr, name, city;
                std::getline(ss, idStr, '|');
                std::getline(ss, name, '|');
                std::getline(ss, city, '|');
                Customer c{std::stoi(idStr), name, city};
                if (index.insert(c.id, c)) customers.push_back(c);
            } catch (...) {
                std::cout << "[warning] Skipped bad line in customers file: " << line << "\n";
            }
        }
        return true;
    }

    void save() const {
        std::ofstream out(filePath);
        for (int i = 0; i < customers.size(); i++)
            out << customers[i].id << "|" << customers[i].name << "|" << customers[i].city << "\n";
    }

    // ---------- accessors used by other modules ----------
    int count() const { return customers.size(); }
    const DynamicArray<Customer>& all() const { return customers; }
    const Customer* find(int id) const { return index.find(id); }          // BST lookup, O(h)

    // ---------- operations ----------
    bool add(const Customer& c) {
        if (!index.insert(c.id, c)) return false;                           // duplicate ID
        customers.push_back(c);
        save();
        return true;
    }

    bool update(int id, const std::string& name, const std::string& city) {
        Customer* inTree = index.find(id);
        int pos = arrayIndexOf(id);
        if (!inTree || pos < 0) return false;
        inTree->name = name;  inTree->city = city;
        customers[pos].name = name;  customers[pos].city = city;
        save();
        return true;
    }

    bool remove(int id) {
        int pos = arrayIndexOf(id);
        if (pos < 0) return false;
        customers.removeAt(pos);
        index.remove(id);
        save();
        return true;
    }

    // ---------- menu ----------
    void viewAll() const {
        Console::header("ALL CUSTOMERS (file order)");
        printHeaderRow();
        for (int i = 0; i < customers.size(); i++) {
            printRow(customers[i]);
            if ((i + 1) % Console::PAGE_SIZE == 0 && i + 1 < customers.size() && !Console::continuePaging()) break;
        }
        std::cout << "\n  Total: " << customers.size() << " customers\n";
    }

    void viewSorted() const {
        Console::header("CUSTOMERS SORTED BY ID");
        std::cout << "  (recursive in-order traversal of the BST)\n\n";
        printHeaderRow();
        int shown = 0;
        bool stopped = false;
        index.inorder([&](const int&, const Customer& c) {
            if (stopped) return;
            printRow(c);
            shown++;
            if (shown % Console::PAGE_SIZE == 0 && shown < index.size() && !Console::continuePaging()) stopped = true;
        });
        std::cout << "\n  Tree: " << index.size() << " nodes, height " << index.height() << "\n";
    }

    void searchById() const {
        int id = Console::readInt("  Customer ID: ");
        DynamicArray<int> path;
        const Customer* c = index.find(id, &path);
        std::cout << "\n  BST search path: ";
        for (int i = 0; i < path.size(); i++) std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
        std::cout << "  (" << path.size() << " comparisons)\n\n";
        if (!c) { std::cout << "  No customer with ID " << id << ".\n"; return; }
        printHeaderRow();
        printRow(*c);
    }

    void addInteractive() {
        int id = Console::readInt("  New customer ID: ");
        if (index.find(id)) { std::cout << "  That ID already exists.\n"; return; }
        std::string name = Console::readText("  Name: ");
        std::string city = Console::readText("  City: ");
        add(Customer{id, name, city});
        std::cout << "  Customer added.\n";
    }

    void updateInteractive() {
        int id = Console::readInt("  Customer ID to update: ");
        const Customer* c = index.find(id);
        if (!c) { std::cout << "  No customer with ID " << id << ".\n"; return; }
        std::cout << "  Current: " << c->name << ", " << c->city << "\n";
        std::string name = Console::readText("  New name: ");
        std::string city = Console::readText("  New city: ");
        update(id, name, city);
        std::cout << "  Customer updated.\n";
    }

    void deleteInteractive() {
        int id = Console::readInt("  Customer ID to delete: ");
        if (remove(id)) std::cout << "  Customer deleted.\n";
        else std::cout << "  No customer with ID " << id << ".\n";
    }

    void menu() {
        while (true) {
            Console::header("CUSTOMER RECORDS");
            std::cout << "    1. View all customers\n    2. View sorted by ID (BST in-order)\n"
                         "    3. Search customer by ID\n    4. Add customer\n"
                         "    5. Update customer\n    6. Delete customer\n\n    7. Back\n";
            int c = Console::readChoice();
            switch (c) {
                case 1: viewAll(); break;
                case 2: viewSorted(); break;
                case 3: searchById(); break;
                case 4: addInteractive(); break;
                case 5: updateInteractive(); break;
                case 6: deleteInteractive(); break;
                case 7: return;
                default: std::cout << "  Invalid choice.\n"; continue;
            }
            Console::pause();
        }
    }
};
