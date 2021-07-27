#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <queue>
#include <vector>

using namespace std;

int n; // number of vertices
int s = 0; // source
int t; // sink
vector<vector<int> > graph; // graph is a matrix
int *pi; // parents of vertices

/*
 * BFS algorithm
 * return value of path flow
 */
int bfs() {
    queue<int> queue;
    int path_capacity[n];

    memset(pi, -1, sizeof(int)*n);
    memset(path_capacity, 0, sizeof(path_capacity));

    // source
    queue.push(s);
    pi[s] = -2;
    path_capacity[s] = INT_MAX;
 
    while (!queue.empty()) {
        int u = queue.front();
        queue.pop();
        // visit vertices v adjacent to u
        for (int v = 0; v < n; v++) {
            // if vertex is not visited, pi = -1
            if (pi[v] == -1 && graph[u][v] > 0) {
                pi[v] = u;
                path_capacity[v] = min(path_capacity[u], graph[u][v]);
                // no need to continue if found a path to t
                if (v == t)
                    return path_capacity[t];
                queue.push(v);
            }
        }
    }
    return 0;
}

/*
 * Edmonds Karp algorithm
 * return maximum flow of graph
 */
int edmondsKarp() {
    int u, v, max_flow = 0, path_flow;

    pi = new int[n];

    // augment the flow while there is a path from source to sink
    while ((path_flow = bfs()) != 0) {
        v = t;
        // update residual capacities of the edges
        while (v != s) {
            u = pi[v];
            graph[u][v] -= path_flow;
            graph[v][u] += path_flow;
            v = u;
        }
        // add path flow to final max flow
        max_flow += path_flow;
    }
    return max_flow;
}

/*
 * MAIN
 */
int main() {
    int k, i, j, temp;

    // read input n and k
    if (scanf("%d %d", &n, &k) < 1)
        return 1;

    // vertices are the n processes + X + Y
    graph.resize(n+2, vector<int>(n+2, 0));

    // sink is the last vertex
    t = n+1;

    // edges from source (X) to process
    // and from process to sink (Y)
    for (int w = 1; w <= n; w++) {
        if (scanf("%d %d", &graph[s][w], &graph[w][t]) < 1)
            return 1;
    }

    // update total number of vertices
    n += 2;

    // edges between processes
    for (int w = 0; w < k; w++) {
        if (scanf("%d %d %d", &i, &j, &temp) < 1)
            return 1;
        graph[i][j] = temp;
        graph[j][i] = temp;
    }

    printf("%d\n", edmondsKarp());

    return 0;
}