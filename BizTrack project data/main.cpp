// BizTrack - Business Records Management System
// CS2003 Data Structures & Business Applications
//
// Build : g++ -std=c++17 -I. main.cpp -o biztrack
// Run   : ./biztrack            (reads data/)
//         ./biztrack data_small (reads the tiny test data set)
#include <iostream>
#include <string>

#include "modules/CustomerManager.h"
#include "modules/InventoryManager.h"
#include "modules/TransactionManager.h"
#include "modules/Analytics.h"
#include "modules/SupplierNetwork.h"
#include "modules/DataStructureLab.h"
#include "utils/Console.h"

int main(int argc, char* argv[]) {
    std::string dir = (argc > 1) ? argv[1] : "data";

    CustomerManager customers(dir + "/customers.txt");
    InventoryManager inventory(dir + "/products.txt");
    TransactionManager transactions(dir + "/transactions.txt", customers, inventory);
    SupplierNetwork network(dir + "/network.txt");

    customers.load();
    inventory.load();
    transactions.load();
    network.load();

    Analytics analytics(customers, inventory, transactions);
    DataStructureLab lab(customers, inventory, network);

    while (true) {
        std::cout << "\n========================================\n"
                     "                 BIZTRACK\n"
                     "        BUSINESS RECORDS MANAGEMENT\n"
                     "========================================\n";
        analytics.dashboard();
        std::cout << "----------------------------------------\n"
                     "    1. Customer Records\n"
                     "    2. Product & Inventory\n"
                     "    3. Sales & Transactions\n"
                     "    4. Business Analytics\n"
                     "    5. Supplier Network\n"
                     "    6. Data Structure Lab\n"
                     "    7. Exit\n"
                     "========================================\n";
        int choice = Console::readChoice("    Enter your choice: ");
        switch (choice) {
            case 1: customers.menu(); break;
            case 2: inventory.menu(); break;
            case 3: transactions.menu(); break;
            case 4: analytics.menu(); break;
            case 5: network.menu(); break;
            case 6: lab.menu(); break;
            case 7: std::cout << "\n  Goodbye!\n"; return 0;
            default: std::cout << "  Invalid choice.\n";
        }
    }
}
