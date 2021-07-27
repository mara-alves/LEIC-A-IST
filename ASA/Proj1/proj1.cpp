#include <cstdio>
#include <vector>
#include <queue>

using namespace std;

/*
 * Find k and l of a DAG
 */
void algorithm(int n, vector<int> inwardCount, vector<vector<int> > outward) {
    int l = 0;
    queue<int> queue;
    vector<int> top_order;
    vector<int> dist(n);

    /* Start of topological sort */

    // Vertices without incoming edges
    for(int i = 0; i < n; i++) {
        if(inwardCount[i] == 0)
            queue.push(i);
    }
        
    // Pieces that must be knocked down (k)
    // = vertices without incoming edges
    printf("%ld ", queue.size());

    while(!queue.empty()) {
        int v = queue.front();
        top_order.push_back(v);
        queue.pop();
        for(int u : outward[v]) {
            inwardCount[u]--;
            if(inwardCount[u] == 0) {
                queue.push(u);
            }
        }
    }
    /* End of topological sort */

    // Relax outgoing edges of v
    for(int v : top_order) {
        for(int u : outward[v]) {
            dist[u] = max(dist[u], dist[v] + 1);
            if(dist[u] > l)
                l = dist[u];
        }
    }

    // Longest sequence of domino pieces that fall (l) 
    // = edges of longest path + 1
    printf("%d\n", l+1);
}

/*
 * MAIN
 */
int main() {
    int n, m, x, y;

    // Read input n and m
    if(scanf("%d %d", &n, &m) < 1)
        return 1;
    
    // Invalid n or m
    if(n < 2 || m < 0)
        return 1;

    vector<int> inwardCount(n);
    vector<vector<int> > outward(n);

    // Read edges from input
    for(int i = 0; i < m; i++) {
        if (scanf("%d %d", &x, &y) < 1)
            return 1;
        inwardCount[y-1]++;
        outward[x-1].push_back(y-1);
    }

    algorithm(n, inwardCount, outward);

    return 0;
}