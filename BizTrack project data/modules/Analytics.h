#pragma once
// ---------------------------------------------------------------
// Analytics  -  simple figures calculated from the loaded records.
// Revenue counts COMPLETED transactions only.
// ---------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include "modules/CustomerManager.h"
#include "modules/InventoryManager.h"
#include "modules/TransactionManager.h"
#include "utils/Console.h"

class Analytics {
    const CustomerManager& customers;
    const InventoryManager& inventory;
    const TransactionManager& transactions;

public:
    Analytics(const CustomerManager& c, const InventoryManager& i, const TransactionManager& t)
        : customers(c), inventory(i), transactions(t) {}

    long long totalRevenue() const {
        long long sum = 0;
        const DynamicArray<Transaction>& all = transactions.all();
        for (int i = 0; i < all.size(); i++)
            if (all[i].status == "COMPLETED") sum += all[i].amount;
        return sum;
    }

    // Returns index of the highest-value COMPLETED transaction, or -1.
    int highestValueIndex() const {
        const DynamicArray<Transaction>& all = transactions.all();
        int best = -1;
        for (int i = 0; i < all.size(); i++)
            if (all[i].status == "COMPLETED" && (best == -1 || all[i].amount > all[best].amount)) best = i;
        return best;
    }

    void dashboard() const {
        std::cout << "              BUSINESS OVERVIEW\n\n";
        std::cout << "    Total Customers       : " << customers.count() << "\n";
        std::cout << "    Total Products        : " << inventory.count() << "\n";
        std::cout << "    Total Transactions    : " << transactions.count() << "\n";
        std::cout << "    Total Revenue         : " << Console::money(totalRevenue()) << "\n";
    }

    void menu() const {
        Console::header("BUSINESS ANALYTICS");
        std::cout << "  Total revenue (completed) : " << Console::money(totalRevenue()) << "\n";
        std::cout << "  Total transactions        : " << transactions.count() << "\n";

        int completed = 0, pendingN = 0, failed = 0;
        const DynamicArray<Transaction>& all = transactions.all();
        for (int i = 0; i < all.size(); i++) {
            if (all[i].status == "COMPLETED") completed++;
            else if (all[i].status == "PENDING") pendingN++;
            else failed++;
        }
        std::cout << "     completed / pending / failed : " << completed << " / " << pendingN << " / " << failed << "\n";
        std::cout << "  Total customers           : " << customers.count() << "\n";
        std::cout << "  Total products            : " << inventory.count() << "\n";

        int best = highestValueIndex();
        std::cout << "\n  Highest-value transaction:\n";
        if (best < 0) std::cout << "    (none completed yet)\n";
        else { TransactionManager::printHeaderRow(); TransactionManager::printRow(all[best]); }

        inventory.viewLowStock();
        Console::pause();
    }
};
