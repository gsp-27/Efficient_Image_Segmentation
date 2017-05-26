#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>

class UnionFind {
private:
    std::vector<int> ranks;
    std::vector<int> components;
    std::vector<int> max_edge;
    const int k = 8000;
public:
    std::vector<int> parent;

    // functions
    UnionFind(int n);
    int find(int v);
    bool unionByRank(int u, int v, int c);
};

#endif // UNIONFIND_H
