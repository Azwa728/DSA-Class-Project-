# BizTrack - Business Records Management System

CS2003 Data Structures & Business Applications - Semester Project
Standard C++17 console application. All data structures and algorithms are implemented manually (no `std::vector`, `std::queue`, `std::sort`, etc.).

**Group members**

| Member | Responsibility |
|---|---|
| _Eshaal_ | DynamicArray, Customer module, BST |
| _Abeer_ | Products / Inventory, searching, sorting & benchmarking |
| _Azwa_ | Transactions, Queue, Stack, Analytics |
| _Zainab_ | Graph, BFS / DFS / Dijkstra, Supplier Network |

## What it does

BizTrack manages the records of a small retail / distribution business: customers, products and stock, sales transactions, and a network of suppliers / warehouses / delivery locations. A separate **Data Structure Lab** lets you operate each structure interactively.

## Build and run

The project has no third-party dependencies. It uses the text files already included in `data/` (or `data_small/`) as its storage.

From the project root, compile with a C++17 compiler:

**Windows (PowerShell):**

```powershell
g++ -std=c++17 -Wall -Wextra -I. -static -static-libgcc -static-libstdc++ main.cpp -o biztrack.exe
.\biztrack.exe                 # full data set
.\biztrack.exe data_small      # small demo data set
```

**Linux / macOS:**

```sh
g++ -std=c++17 -Wall -Wextra -I. main.cpp -o biztrack
./biztrack                      # full data set
./biztrack data_small           # small demo data set
```

Run from the project root so the data paths resolve. Changes made in the app are saved to the selected data files immediately. Back up the files before a demo if you want to preserve the original records.

To share the Windows build, include `biztrack.exe`, `data/`, and `data_small/` in the zip. The recipient can extract the folder and run the executable from that folder on 64-bit Windows 10 or later; they do not need a C++ compiler or MSYS2 installed.

## Folder structure

```
BizTrack/
├── main.cpp                  main menu + dashboard, wires the modules together
├── models/                   plain record types: Customer, Product, Transaction
├── structures/               the data structures (header-only templates)
│   ├── DynamicArray.h        resizable array
│   ├── Queue.h               FIFO queue (circular array)
│   ├── Stack.h               LIFO stack
│   ├── BST.h                 binary search tree
│   └── Graph.h               adjacency-list graph + BFS, DFS, Dijkstra
├── utils/                    Search.h, Sorting.h, Timer.h, Console.h
├── modules/                  business features + the Lab (menus and logic)
├── data/                     mock data used by default
└── data_small/               small data set for demos and tracing
```

Dependency direction: `main` -> `modules` -> (`structures`, `utils`, `models`). Structures never know about business classes.

## Data structures and algorithms

| Course module | Topic | Where it is used |
|---|---|---|
| Linear Structures | Dynamic Array | Stores customers, products, transactions; graph adjacency lists |
| Linear Structures | Queue | Pending transactions (FIFO); BFS frontier |
| Linear Structures | Stack | Lab push/pop demonstration |
| Hierarchical | Binary Search Tree | Customer index by ID; sorted listing |
| Graph Algorithms | Adjacency list, BFS, DFS | Supplier network, reachable locations |
| Graph Algorithms | Dijkstra | Lowest-cost delivery route |
| Data Processing | Linear + binary search | Product / transaction lookup, Lab comparison |
| Data Processing | Insertion sort + merge sort | Product sorting, benchmarking |
| Data Processing | Recursion | BST operations/traversals, DFS, merge sort |

### Deliberately excluded (and why)

| Topic | Reason |
|---|---|
| Linked lists | Records are mostly appended and read in bulk; a dynamic array has better locality and is simpler. The queue uses a circular array for the same reason. |
| AVL / B-Trees | A plain BST is enough at this scale (~150 customers); file order is shuffled so the tree stays reasonably shallow. |
| Heaps / priority queues | Pending transactions are strictly first-come-first-served, so no priority is needed. Dijkstra uses a linear scan for 12 vertices. |
| Bellman-Ford | All delivery costs are non-negative, so Dijkstra is correct and faster. |
| Minimum spanning tree | The business needs routes between two locations, not the cheapest network connecting all of them. |

## Data files (`data/`, `|`-separated)

| File | Fields |
|---|---|
| `customers.txt` | id, name, city |
| `products.txt` | id, name, category, price, stock |
| `transactions.txt` | id, customerId, productId, quantity, amount, status (`PENDING` / `COMPLETED` / `FAILED`) |
| `network.txt` | locationA, locationB, cost (undirected) |

Rules enforced by the program: a transaction must reference an existing customer and product; `amount = price x quantity`; processing a pending transaction checks stock, reduces it on success and marks the transaction `COMPLETED`, otherwise `FAILED`. Revenue counts completed transactions only. Low stock means stock below 10.

## Known limitations

- Deleting a customer or product does not remove their old transactions; a pending transaction that refers to a deleted record fails when processed.
- Console only, no authentication, no concurrency (by design, see project scope).
