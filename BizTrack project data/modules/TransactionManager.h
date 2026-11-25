#pragma once
// ---------------------------------------------------------------
// TransactionManager  -  sales records.
//   DynamicArray<Transaction> : every transaction (file order = order added)
//   Queue<std::string>        : IDs of PENDING transactions, FIFO
//
// Recording a transaction validates the customer (BST lookup) and the
// product, computes amount = price * quantity and enqueues it as PENDING.
// Processing dequeues the oldest pending one, checks stock, and marks it
// COMPLETED (stock reduced) or FAILED (not enough stock / missing record).
// ---------------------------------------------------------------
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "models/Transaction.h"
#include "structures/DynamicArray.h"
#include "structures/Queue.h"
#include "modules/CustomerManager.h"
#include "modules/InventoryManager.h"
#include "utils/Search.h"
#include "utils/Console.h"

class TransactionManager {
    DynamicArray<Transaction> txns;
    Queue<std::string> pending;
    std::string filePath;
    CustomerManager& customers;
    InventoryManager& inventory;

    std::string nextId() const {
        int maxNum = 1000;
        for (int i = 0; i < txns.size(); i++) {
            try {
                int n = std::stoi(txns[i].id.substr(1));
                if (n > maxNum) maxNum = n;
            } catch (...) {}
        }
        return "T" + std::to_string(maxNum + 1);
    }

    int indexOfId(const std::string& id) const {
        return linearSearch(txns, [&](const Transaction& t) { return t.id == id; });
    }

public:
    TransactionManager(const std::string& path, CustomerManager& cm, InventoryManager& im)
        : filePath(path), customers(cm), inventory(im) {}

    static void printHeaderRow() {
        std::cout << "  " << std::left << std::setw(8) << "ID" << std::setw(10) << "Customer"
                  << std::setw(10) << "Product" << std::right << std::setw(5) << "Qty"
                  << std::setw(16) << "Amount" << "  " << std::left << "Status\n";
        std::cout << "  " << std::string(62, '-') << "\n";
    }
    static void printRow(const Transaction& t, const char* marker = "  ") {
        std::cout << marker << std::left << std::setw(8) << t.id << std::setw(10) << t.customerId
                  << std::setw(10) << t.productId << std::right << std::setw(5) << t.quantity
                  << std::setw(16) << Console::money(t.amount) << "  " << std::left << t.status << "\n";
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
                Transaction t;
                std::string cid, qty, amt;
                std::getline(ss, t.id, '|');
                std::getline(ss, cid, '|');
                std::getline(ss, t.productId, '|');
                std::getline(ss, qty, '|');
                std::getline(ss, amt, '|');
                std::getline(ss, t.status, '|');
                t.customerId = std::stoi(cid);
                t.quantity = std::stoi(qty);
                t.amount = std::stoll(amt);
                txns.push_back(t);
                if (t.status == "PENDING") pending.enqueue(t.id);    // file order = arrival order
            } catch (...) {
                std::cout << "[warning] Skipped bad line in transactions file: " << line << "\n";
            }
        }
        return true;
    }

    void save() const {
        std::ofstream out(filePath);
        for (int i = 0; i < txns.size(); i++) {
            const Transaction& t = txns[i];
            out << t.id << "|" << t.customerId << "|" << t.productId << "|" << t.quantity
                << "|" << t.amount << "|" << t.status << "\n";
        }
    }

    // ---------- accessors used by Analytics ----------
    int count() const { return txns.size(); }
    int pendingCount() const { return pending.size(); }
    const DynamicArray<Transaction>& all() const { return txns; }

    // ---------- menu actions ----------
    void viewAll() const {
        Console::header("ALL TRANSACTIONS");
        printHeaderRow();
        for (int i = 0; i < txns.size(); i++) {
            printRow(txns[i]);
            if ((i + 1) % Console::PAGE_SIZE == 0 && i + 1 < txns.size() && !Console::continuePaging()) break;
        }
        std::cout << "\n  Total: " << txns.size() << " transactions\n";
    }

    void searchById() const {
        std::string id = Console::toUpper(Console::readText("  Transaction ID (e.g. T1001): "));
        long comparisons = 0;
        int i = linearSearch(txns, [&](const Transaction& t) { return t.id == id; }, &comparisons);
        std::cout << "\n";
        if (i < 0) { std::cout << "  No transaction with ID " << id << ".\n"; return; }
        printHeaderRow();
        printRow(txns[i]);
        std::cout << "\n  (found after " << comparisons << " comparisons)\n";
    }

    void recordInteractive() {
        int cid = Console::readInt("  Customer ID: ");
        const Customer* c = customers.find(cid);
        if (!c) { std::cout << "  No customer with ID " << cid << ". Transaction cancelled.\n"; return; }
        std::cout << "  Customer: " << c->name << "\n";

        std::string pid = Console::toUpper(Console::readText("  Product ID: "));
        Product* p = inventory.find(pid);
        if (!p) { std::cout << "  No product with ID " << pid << ". Transaction cancelled.\n"; return; }
        std::cout << "  Product : " << p->name << " (" << Console::money(p->price) << ", stock " << p->stock << ")\n";

        int qty = Console::readIntInRange("  Quantity: ", 1, 100000);
        Transaction t;
        t.id = nextId();
        t.customerId = cid;
        t.productId = pid;
        t.quantity = qty;
        t.amount = (long long)p->price * qty;
        t.status = "PENDING";
        txns.push_back(t);
        pending.enqueue(t.id);
        save();
        std::cout << "\n  Recorded " << t.id << " for " << Console::money(t.amount)
                  << ". Added to the pending queue (position " << pending.size() << ").\n";
    }

    void viewPending() const {
        Console::header("PENDING TRANSACTIONS (FIFO)");
        if (pending.isEmpty()) { std::cout << "  The queue is empty.\n"; return; }
        printHeaderRow();
        for (int i = 0; i < pending.size(); i++) {
            int idx = indexOfId(pending.at(i));
            if (idx >= 0) printRow(txns[idx], i == 0 ? "->" : "  ");
        }
        std::cout << "\n  " << pending.size() << " waiting.  '->' marks the next one to be processed.\n";
    }

    void processNext() {
        if (pending.isEmpty()) { std::cout << "  No pending transactions.\n"; return; }
        std::string id = pending.dequeue();                           // oldest first
        int idx = indexOfId(id);
        if (idx < 0) { std::cout << "  Internal error: " << id << " not found.\n"; return; }
        Transaction& t = txns[idx];

        std::cout << "  Processing " << t.id << " (customer " << t.customerId << ", " << t.productId
                  << " x" << t.quantity << ")\n";
        Product* p = inventory.find(t.productId);
        if (!customers.find(t.customerId)) {
            t.status = "FAILED";
            std::cout << "  FAILED: customer no longer exists.\n";
        } else if (!p) {
            t.status = "FAILED";
            std::cout << "  FAILED: product no longer exists.\n";
        } else if (p->stock < t.quantity) {
            t.status = "FAILED";
            std::cout << "  FAILED: only " << p->stock << " in stock, " << t.quantity << " requested.\n";
        } else {
            p->stock -= t.quantity;
            t.status = "COMPLETED";
            inventory.save();
            std::cout << "  COMPLETED: stock of " << p->name << " reduced to " << p->stock << ".\n";
        }
        save();
        std::cout << "  " << pending.size() << " transaction(s) still pending.\n";
    }

    void menu() {
        while (true) {
            Console::header("SALES & TRANSACTIONS");
            std::cout << "    1. View all transactions\n    2. Search transaction by ID\n"
                         "    3. Record new transaction\n    4. View pending transactions\n"
                         "    5. Process next pending transaction\n\n    6. Back\n";
            int c = Console::readChoice();
            switch (c) {
                case 1: viewAll(); break;
                case 2: searchById(); break;
                case 3: recordInteractive(); break;
                case 4: viewPending(); break;
                case 5: processNext(); break;
                case 6: return;
                default: std::cout << "  Invalid choice.\n"; continue;
            }
            Console::pause();
        }
    }
};
