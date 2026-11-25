#pragma once
// ---------------------------------------------------------------
// DataStructureLab  -  interactive demonstrations of the structures and
// algorithms. It reuses the SAME classes the business modules use
// (DynamicArray, Queue, Stack, BST, Graph, search and sort templates).
// ---------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <string>
#include "structures/DynamicArray.h"
#include "structures/Queue.h"
#include "structures/Stack.h"
#include "structures/BST.h"
#include "utils/Search.h"
#include "utils/Sorting.h"
#include "utils/Timer.h"
#include "utils/Console.h"
#include "modules/CustomerManager.h"
#include "modules/InventoryManager.h"
#include "modules/SupplierNetwork.h"

class DataStructureLab {
    const CustomerManager& customers;
    const InventoryManager& inventory;
    const SupplierNetwork& network;

    // ================= 1. LINEAR STRUCTURES =================
    void dynamicArrayDemo() {
        DynamicArray<int> arr(2);                       // tiny capacity so growth is easy to see
        while (true) {
            Console::header("DYNAMIC ARRAY DEMONSTRATION");
            std::cout << "  Elements: [";
            for (int i = 0; i < arr.size(); i++) std::cout << arr[i] << (i + 1 < arr.size() ? ", " : "");
            std::cout << "]\n  Size: " << arr.size() << "   Capacity: " << arr.capacity()
                      << "   (capacity doubles when full)\n\n";
            std::cout << "    1. Append element\n    2. Insert at index\n    3. Remove at index\n    4. Back\n";
            int c = Console::readChoice();
            if (c == 1) {
                arr.push_back(Console::readInt("  Value: "));
            } else if (c == 2) {
                int idx = Console::readInt("  Index: ");
                int v = Console::readInt("  Value: ");
                try { arr.insertAt(idx, v); } catch (...) { std::cout << "  Index must be 0.." << arr.size() << ".\n"; }
            } else if (c == 3) {
                int idx = Console::readInt("  Index: ");
                try { arr.removeAt(idx); } catch (...) { std::cout << "  Index must be 0.." << arr.size() - 1 << ".\n"; }
            } else if (c == 4) return;
        }
    }

    void queueDemo() {
        Queue<int> q(2);
        while (true) {
            Console::header("QUEUE DEMONSTRATION (FIFO)");
            if (q.isEmpty()) std::cout << "  Queue is empty\n";
            else {
                std::cout << "  FRONT -> ";
                for (int i = 0; i < q.size(); i++) std::cout << "[" << q.at(i) << "] ";
                std::cout << "<- REAR\n";
            }
            std::cout << "\n    1. Enqueue\n    2. Dequeue\n    3. Back\n";
            int c = Console::readChoice();
            if (c == 1) q.enqueue(Console::readInt("  Value: "));
            else if (c == 2) {
                if (q.isEmpty()) std::cout << "  Queue is empty - nothing to dequeue.\n";
                else std::cout << "  Dequeued " << q.dequeue() << "\n";
            } else if (c == 3) return;
        }
    }

    void stackDemo() {
        Stack<int> s;
        while (true) {
            Console::header("STACK DEMONSTRATION (LIFO)");
            std::cout << "  Current Stack:\n\n";
            if (s.isEmpty()) std::cout << "    (empty)\n";
            for (int i = s.size() - 1; i >= 0; i--)
                std::cout << (i == s.size() - 1 ? "    TOP -> [" : "           [") << s.at(i) << "]\n";
            std::cout << "\n    1. Push Element\n    2. Pop Element\n    3. Back\n";
            int c = Console::readChoice();
            if (c == 1) s.push(Console::readInt("  Value: "));
            else if (c == 2) {
                if (s.isEmpty()) std::cout << "  Stack is empty - nothing to pop.\n";
                else std::cout << "  Popped " << s.pop() << "\n";
            } else if (c == 3) return;
        }
    }

    void linearMenu() {
        while (true) {
            Console::header("LINEAR STRUCTURES");
            std::cout << "    1. Dynamic Array\n    2. Queue\n    3. Stack\n\n    4. Back\n";
            int c = Console::readChoice();
            if (c == 1) dynamicArrayDemo();
            else if (c == 2) queueDemo();
            else if (c == 3) stackDemo();
            else if (c == 4) return;
        }
    }

    // ================= 2. HIERARCHICAL: BST =================
    void bstDemo() {
        BST<int, int> tree;
        while (true) {
            Console::header("BINARY SEARCH TREE DEMONSTRATION");
            std::cout << "  Nodes: " << tree.size() << "   Height: " << tree.height() << "\n\n";
            std::cout << "    1. Insert key            5. Pre-order traversal\n"
                         "    2. Search key            6. Post-order traversal\n"
                         "    3. Delete key            7. Draw tree (sideways)\n"
                         "    4. In-order traversal    8. Trace recursive in-order\n"
                         "    9. Load sample (50,30,70,20,40,60,80)\n"
                         "   10. Build from customer IDs\n"
                         "   11. Back\n";
            int c = Console::readChoice();
            auto printKey = [](const int& k, const int&) { std::cout << k << " "; };
            if (c == 1) {
                int k = Console::readInt("  Key: ");
                std::cout << (tree.insert(k, k) ? "  Inserted.\n" : "  Key already exists.\n");
            } else if (c == 2) {
                int k = Console::readInt("  Key: ");
                DynamicArray<int> path;
                bool found = tree.find(k, &path) != nullptr;
                std::cout << "  Path: ";
                for (int i = 0; i < path.size(); i++) std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
                std::cout << "\n  " << (found ? "FOUND" : "NOT FOUND") << " after " << path.size() << " comparisons.\n";
            } else if (c == 3) {
                int k = Console::readInt("  Key: ");
                std::cout << (tree.remove(k) ? "  Deleted.\n" : "  Key not found.\n");
            } else if (c == 4) { std::cout << "  In-order  : "; tree.inorder(printKey); std::cout << "\n"; }
            else if (c == 5) { std::cout << "  Pre-order : "; tree.preorder(printKey); std::cout << "\n"; }
            else if (c == 6) { std::cout << "  Post-order: "; tree.postorder(printKey); std::cout << "\n"; }
            else if (c == 7) {
                if (tree.size() > 31) std::cout << "  Tree too large to draw (" << tree.size() << " nodes).\n";
                else { std::cout << "  (root on the left, right children above, left children below)\n\n"; tree.printSideways(); }
            } else if (c == 8) {
                if (tree.size() > 15) std::cout << "  Trace is too long for " << tree.size() << " nodes; use 15 or fewer.\n";
                else tree.traceInorder();
            } else if (c == 9) {
                tree.clear();
                int keys[] = {50, 30, 70, 20, 40, 60, 80};
                for (int i = 0; i < 7; i++) tree.insert(keys[i], keys[i]);
                std::cout << "  Sample tree loaded.\n";
            } else if (c == 10) {
                tree.clear();
                const DynamicArray<Customer>& all = customers.all();
                for (int i = 0; i < all.size(); i++) tree.insert(all[i].id, all[i].id);
                std::cout << "  Inserted " << tree.size() << " customer IDs in file order. Height = " << tree.height()
                          << " (a perfectly balanced tree would be " << (int)(std::log2((double)tree.size())) + 1 << ").\n";
            } else if (c == 11) return;
            if (c >= 1 && c <= 10) Console::pause();
        }
    }

    void hierarchicalMenu() {
        while (true) {
            Console::header("HIERARCHICAL STRUCTURES");
            std::cout << "    1. Binary Search Tree\n\n    2. Back\n";
            int c = Console::readChoice();
            if (c == 1) bstDemo();
            else if (c == 2) return;
        }
    }

    // ================= 3. GRAPH ALGORITHMS =================
    void graphMenu() {
        while (true) {
            Console::header("GRAPH ALGORITHMS");
            std::cout << "    1. Show adjacency list\n    2. BFS / DFS traversal\n"
                         "    3. Dijkstra (with full distance table)\n\n    4. Back\n";
            int c = Console::readChoice();
            if (c == 1) network.viewConnections();
            else if (c == 2) network.showReachable();
            else if (c == 3) network.showRoute(true);
            else if (c == 4) return;
            if (c >= 1 && c <= 3) Console::pause();
        }
    }

    // ================= 4. SEARCHING & SORTING =================
    static DynamicArray<int> randomInts(int n, unsigned seed) {
        std::mt19937 rng(seed);
        DynamicArray<int> a(n);
        for (int i = 0; i < n; i++) a.push_back((int)(rng() % (unsigned)(n * 10)));
        return a;
    }

    static void printSample(const DynamicArray<int>& a, int limit = 12) {
        std::cout << "[";
        for (int i = 0; i < a.size() && i < limit; i++) std::cout << a[i] << (i + 1 < a.size() && i + 1 < limit ? ", " : "");
        if (a.size() > limit) std::cout << ", ...";
        std::cout << "]";
    }

    void searchComparison() {
        Console::header("LINEAR vs BINARY SEARCH");
        int n = Console::readIntInRange("  Number of elements (10 - 1000000): ", 10, 1000000);
        DynamicArray<int> data = randomInts(n, 7);
        int target = data[n - 1];                               // near the end of the unsorted data

        long linCmp = 0;
        linearSearch(data, [&](const int& x) { return x == target; }, &linCmp);
        Timer t1;
        for (int r = 0; r < 200; r++) linearSearch(data, [&](const int& x) { return x == target; });
        double linMs = t1.ms() / 200;

        Timer ts;
        DynamicArray<int> sorted = data;
        mergeSort(sorted, [](const int& a, const int& b) { return a < b; });
        double sortMs = ts.ms();

        long binCmp = 0;
        binarySearch(sorted, target, [](const int& x) { return x; }, &binCmp);
        Timer t2;
        for (int r = 0; r < 200; r++) binarySearch(sorted, target, [](const int& x) { return x; });
        double binMs = t2.ms() / 200;

        std::cout << "\n  Searching for " << target << " in " << n << " numbers:\n\n";
        std::cout << "    Linear search (unsorted): " << std::setw(8) << linCmp << " comparisons, " << linMs << " ms\n";
        std::cout << "    Binary search (sorted)  : " << std::setw(8) << binCmp << " comparisons, " << binMs << " ms\n";
        std::cout << "\n  Binary search needs sorted data. Sorting once cost " << sortMs << " ms (merge sort),\n"
                     "  which pays off when the data is searched many times.\n";
    }

    void sortBenchmark() {
        Console::header("SORTING BENCHMARK");
        int n = Console::readIntInRange("  Number of elements (10 - 100000): ", 10, 100000);
        DynamicArray<int> base = randomInts(n, 11);
        auto lessInt = [](const int& a, const int& b) { return a < b; };

        DynamicArray<int> a = base, b = base;
        Timer t1;  insertionSort(a, lessInt);  double insMs = t1.ms();
        Timer t2;  mergeSort(b, lessInt);      double merMs = t2.ms();

        std::cout << "\n  Before: "; printSample(base);
        std::cout << "\n  After : "; printSample(a);
        std::cout << "\n\n    Insertion sort (O(n^2))    : " << insMs << " ms";
        std::cout << "\n    Merge sort     (O(n log n)): " << merMs << " ms\n";
        std::cout << "\n  Try 1000, 10000 and 50000 to watch the gap grow.\n";
    }

    void productSortDemo() {
        Console::header("PRODUCTS BEFORE / AFTER SORTING");
        DynamicArray<Product> copy = inventory.all();
        int limit = copy.size() < 8 ? copy.size() : 8;
        std::cout << "  BEFORE (first " << limit << ", file order):\n";
        InventoryManager::printHeaderRow();
        for (int i = 0; i < limit; i++) InventoryManager::printRow(copy[i]);
        mergeSort(copy, [](const Product& x, const Product& y) { return x.price < y.price; });
        std::cout << "\n  AFTER merge sort by price (cheapest " << limit << "):\n";
        InventoryManager::printHeaderRow();
        for (int i = 0; i < limit; i++) InventoryManager::printRow(copy[i]);
    }

    void searchSortMenu() {
        while (true) {
            Console::header("SEARCHING & SORTING");
            std::cout << "    1. Linear vs Binary search\n    2. Sorting benchmark (insertion vs merge)\n"
                         "    3. Products before / after sorting\n\n    4. Back\n";
            int c = Console::readChoice();
            if (c == 1) searchComparison();
            else if (c == 2) sortBenchmark();
            else if (c == 3) productSortDemo();
            else if (c == 4) return;
            if (c >= 1 && c <= 3) Console::pause();
        }
    }

public:
    DataStructureLab(const CustomerManager& c, const InventoryManager& i, const SupplierNetwork& n)
        : customers(c), inventory(i), network(n) {}

    void menu() {
        while (true) {
            Console::header("DATA STRUCTURE LAB");
            std::cout << "    1. Linear Structures\n    2. Hierarchical Structures\n"
                         "    3. Graph Algorithms\n    4. Searching & Sorting\n\n    5. Back to Main Menu\n";
            int c = Console::readChoice();
            switch (c) {
                case 1: linearMenu(); break;
                case 2: hierarchicalMenu(); break;
                case 3: graphMenu(); break;
                case 4: searchSortMenu(); break;
                case 5: return;
                default: std::cout << "  Invalid choice.\n";
            }
        }
    }
};
