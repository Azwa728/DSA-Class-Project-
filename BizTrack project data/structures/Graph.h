#pragma once
// ---------------------------------------------------------------
// Graph  -  weighted, UNDIRECTED graph stored as an ADJACENCY LIST.
// Vertices = locations (suppliers / warehouses / cities), identified
// by an integer index; names[] maps index -> name.
// Edge weight = delivery cost.
//
// Algorithms: BFS (uses Queue), DFS (recursive), Dijkstra (linear scan
// for the "closest unvisited vertex" step, so no heap is required).
// ---------------------------------------------------------------
#include <string>
#include "structures/DynamicArray.h"
#include "structures/Queue.h"

struct Edge {
    int to = 0;
    int cost = 0;
};

class Graph {
    DynamicArray<std::string> names;
    DynamicArray<DynamicArray<Edge>> adj;

    void dfsRec(int u, DynamicArray<bool>& visited, DynamicArray<int>& order) const {
        visited[u] = true;
        order.push_back(u);
        const DynamicArray<Edge>& nb = adj[u];
        for (int i = 0; i < nb.size(); i++)
            if (!visited[nb[i].to]) dfsRec(nb[i].to, visited, order);
    }

public:
    static constexpr int INF = 1000000000;

    int indexOf(const std::string& name) const {
        for (int i = 0; i < names.size(); i++)
            if (names[i] == name) return i;
        return -1;
    }

    int addLocation(const std::string& name) {
        int idx = indexOf(name);
        if (idx != -1) return idx;
        names.push_back(name);
        adj.push_back(DynamicArray<Edge>());
        return names.size() - 1;
    }

    // Undirected: store the edge in both vertices' lists.
    void addEdge(const std::string& a, const std::string& b, int cost) {
        int u = addLocation(a), v = addLocation(b);
        if (u == v) return;
        adj[u].push_back(Edge{v, cost});
        adj[v].push_back(Edge{u, cost});
    }

    int locationCount() const { return names.size(); }
    const std::string& nameOf(int i) const { return names[i]; }
    const DynamicArray<Edge>& neighbors(int i) const { return adj[i]; }

    // Breadth-first: visits vertices level by level using a Queue.
    DynamicArray<int> bfs(int start) const {
        DynamicArray<int> order;
        DynamicArray<bool> visited;
        for (int i = 0; i < names.size(); i++) visited.push_back(false);
        Queue<int> q;
        visited[start] = true;
        q.enqueue(start);
        while (!q.isEmpty()) {
            int u = q.dequeue();
            order.push_back(u);
            const DynamicArray<Edge>& nb = adj[u];
            for (int i = 0; i < nb.size(); i++) {
                int v = nb[i].to;
                if (!visited[v]) { visited[v] = true; q.enqueue(v); }
            }
        }
        return order;
    }

    // Depth-first: goes as deep as possible before backtracking (recursive).
    DynamicArray<int> dfs(int start) const {
        DynamicArray<int> order;
        DynamicArray<bool> visited;
        for (int i = 0; i < names.size(); i++) visited.push_back(false);
        dfsRec(start, visited, order);
        return order;
    }

    // Dijkstra from src. Fills dist[] (INF = unreachable) and prev[] (for rebuilding paths).
    void dijkstra(int src, DynamicArray<int>& dist, DynamicArray<int>& prev) const {
        int n = names.size();
        dist.clear();
        prev.clear();
        DynamicArray<bool> done;
        for (int i = 0; i < n; i++) { dist.push_back(INF); prev.push_back(-1); done.push_back(false); }
        dist[src] = 0;

        for (int round = 0; round < n; round++) {
            int u = -1;                                   // closest unfinished vertex (linear scan)
            for (int i = 0; i < n; i++)
                if (!done[i] && dist[i] < INF && (u == -1 || dist[i] < dist[u])) u = i;
            if (u == -1) break;                           // the rest is unreachable
            done[u] = true;
            const DynamicArray<Edge>& nb = adj[u];
            for (int j = 0; j < nb.size(); j++) {         // relax every edge out of u
                int v = nb[j].to;
                int nd = dist[u] + nb[j].cost;
                if (nd < dist[v]) { dist[v] = nd; prev[v] = u; }
            }
        }
    }
};
