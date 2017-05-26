#include "unionfind.h"

UnionFind::UnionFind(int n) {
    ranks.resize(n, 0);
    components.resize(n, 1);
    max_edge.resize(n, 0);
    parent.resize(n);

    // initialize each element to be its own parent
    for(int i=0; i<n; ++i) {
        parent[i] = i;
    }
}

int UnionFind::find(int v) {
    if(parent[v] == v) {
        return v;
    }
    else {
        int result = find(parent[v]);
        parent[v] = result;
        return result;
    }
}

bool UnionFind::unionByRank(int u, int v, int c) {
    int p_u = find(u);
    int p_v = find(v);

    if (p_u == p_v) {
        // same parent definitely not going to merge already in the same component.
        return false;
    }
    else {
        // now there is a possibility of merge if the thresholds are crossed
        // TODO: should I make it float?
        int thresh_u = max_edge[p_u] + (k / components[p_u]);
        int thresh_v = max_edge[p_v] + (k / components[p_v]);
        if(c < thresh_u && c < thresh_v) {
            // now we will merge, if merging we will use heuristic to make it fast;
            int rank_u = ranks[p_u];
            int rank_v = ranks[p_v];
            if(rank_u > rank_v) {
                // the height of u is more, merge v into u
                // since v is merged in u, number of components in u increases.
                parent[p_v] = p_u;
                components[p_u] += components[p_v];
                max_edge[p_u] = c;
            }
            else if(rank_v > rank_u) {
                // the height of v is more, merge u into v
                parent[p_u] = p_v;
                components[p_v] += components[p_u];
                max_edge[p_v] = c;
            }
            else {
                // can merge anything into anything.
                parent[p_v] = p_u;
                ranks[p_u] += 1;
                // since merging in u, everything of u will change
                components[p_u] += components[p_v];
                max_edge[p_u] = c;
            }
        }
        return true;
    }
    return false;
}
