#pragma once
// ---------------------------------------------------------------
// SupplierNetwork  -  suppliers / warehouses / delivery locations as a Graph.
// File format (network.txt):  LocationA|LocationB|cost   (undirected edge)
// ---------------------------------------------------------------
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "structures/Graph.h"
#include "utils/Console.h"

class SupplierNetwork {
    Graph graph;
    std::string filePath;

    void printOrder(const DynamicArray<int>& order) const {
        for (int i = 0; i < order.size(); i++)
            std::cout << graph.nameOf(order[i]) << (i + 1 < order.size() ? " -> " : "\n");
    }

public:
    explicit SupplierNetwork(const std::string& path) : filePath(path) {}

    bool load() {
        std::ifstream file(filePath);
        if (!file) { std::cout << "[warning] Could not open " << filePath << "\n"; return false; }
        std::string line;
        while (std::getline(file, line)) {
            Console::stripCR(line);
            if (line.empty()) continue;
            try {
                std::stringstream ss(line);
                std::string a, b, cost;
                std::getline(ss, a, '|');
                std::getline(ss, b, '|');
                std::getline(ss, cost, '|');
                graph.addEdge(a, b, std::stoi(cost));
            } catch (...) {
                std::cout << "[warning] Skipped bad line in network file: " << line << "\n";
            }
        }
        return true;
    }

    const Graph& getGraph() const { return graph; }

    int pickLocation(const std::string& prompt) const {
        for (int i = 0; i < graph.locationCount(); i++)
            std::cout << "    " << std::right << std::setw(2) << i + 1 << ". " << graph.nameOf(i) << "\n";
        return Console::readIntInRange(prompt, 1, graph.locationCount()) - 1;
    }

    void viewLocations() const {
        Console::header("LOCATIONS");
        for (int i = 0; i < graph.locationCount(); i++)
            std::cout << "  " << std::right << std::setw(2) << i + 1 << ". " << std::left << std::setw(14)
                      << graph.nameOf(i) << graph.neighbors(i).size() << " connection(s)\n";
        std::cout << "\n  Total: " << graph.locationCount() << " locations\n";
    }

    // Prints the adjacency list exactly as it is stored.
    void viewConnections() const {
        Console::header("CONNECTIONS (ADJACENCY LIST)");
        for (int i = 0; i < graph.locationCount(); i++) {
            std::cout << "  " << std::left << std::setw(12) << graph.nameOf(i) << " -> ";
            const DynamicArray<Edge>& nb = graph.neighbors(i);
            for (int j = 0; j < nb.size(); j++)
                std::cout << graph.nameOf(nb[j].to) << "(" << nb[j].cost << ")" << (j + 1 < nb.size() ? ", " : "");
            std::cout << "\n";
        }
    }

    void showReachable() const {
        Console::header("REACHABLE LOCATIONS");
        int start = pickLocation("  Start from: ");
        DynamicArray<int> b = graph.bfs(start), d = graph.dfs(start);
        std::cout << "\n  BFS order (nearest hops first):\n    ";
        printOrder(b);
        std::cout << "\n  DFS order (deepest first):\n    ";
        printOrder(d);
        std::cout << "\n  " << b.size() << " of " << graph.locationCount() << " locations are reachable from "
                  << graph.nameOf(start) << ".\n";
    }

    void showRoute(bool showTable = false) const {
        Console::header("LOWEST-COST ROUTE");
        std::cout << "  From:\n";
        int src = pickLocation("  Choose start: ");
        std::cout << "  To:\n";
        int dst = pickLocation("  Choose destination: ");

        DynamicArray<int> dist, prev;
        graph.dijkstra(src, dist, prev);

        if (showTable) {
            std::cout << "\n  Dijkstra result from " << graph.nameOf(src) << ":\n";
            for (int i = 0; i < graph.locationCount(); i++) {
                std::cout << "    " << std::left << std::setw(12) << graph.nameOf(i);
                if (dist[i] >= Graph::INF) std::cout << "unreachable\n";
                else std::cout << std::right << std::setw(6) << dist[i] << "   (came from "
                               << (prev[i] < 0 ? "-" : graph.nameOf(prev[i])) << ")\n";
            }
        }

        std::cout << "\n";
        if (dist[dst] >= Graph::INF) { std::cout << "  No route exists.\n"; return; }
        DynamicArray<int> rev;
        for (int v = dst; v != -1; v = prev[v]) rev.push_back(v);   // walk back to the source
        std::cout << "  Cheapest route: ";
        for (int i = rev.size() - 1; i >= 0; i--)
            std::cout << graph.nameOf(rev[i]) << (i > 0 ? " -> " : "\n");
        std::cout << "  Total cost    : " << dist[dst] << "\n";
    }

    void menu() const {
        while (true) {
            Console::header("SUPPLIER NETWORK");
            std::cout << "    1. View all locations\n    2. View connections\n"
                         "    3. Find reachable locations (BFS / DFS)\n"
                         "    4. Find lowest-cost route (Dijkstra)\n\n    5. Back\n";
            int c = Console::readChoice();
            switch (c) {
                case 1: viewLocations(); break;
                case 2: viewConnections(); break;
                case 3: showReachable(); break;
                case 4: showRoute(); break;
                case 5: return;
                default: std::cout << "  Invalid choice.\n"; continue;
            }
            Console::pause();
        }
    }
};
