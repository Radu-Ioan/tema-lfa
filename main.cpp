#include <iostream>
#include <fstream>
#include <vector>
#include <list>

#include "synchronize.h"

using namespace std;

#define DISCOVERED_FLAG (uint8_t) 1
#define VISITED (uint8_t) 3
#define PRODUCTIVE_FLAG (uint8_t) 4
#define FINAL_STATE_FLAG (uint8_t) 8

inline bool is_discovered(int u, const vector<uint8_t> &masks)
{
    return masks[u] & DISCOVERED_FLAG;
}

inline bool is_visited(int u, const vector<uint8_t> &masks)
{
    return masks[u] & VISITED;
}

inline bool is_productive(int u, const vector<uint8_t> &masks)
{
    return masks[u] & PRODUCTIVE_FLAG;
}

inline bool is_final_state(int u, const vector<uint8_t> &masks)
{
    return masks[u] & FINAL_STATE_FLAG;
}

void accessible_dfs(int u, const vector<vector<int>> &edge,
                    vector<uint8_t> &states_masks)
{
    list<int> stack = {u};

    while (!stack.empty()) {
        int v = stack.front();
        stack.pop_front();
        states_masks[v] |= DISCOVERED_FLAG;

        for (size_t c = 0; c < edge[v].size(); c++) {
            int w = edge[v][c];

            if (!is_discovered(w, states_masks)) {
                stack.push_front(w);
            }
        }
    }
}

void find_accessible_states(vector<vector<int>> &edge,
                            vector<int> &start_states,
                            vector<uint8_t> &states_masks)
{
    for (int u : start_states)
        if (!is_discovered(u, states_masks))
            accessible_dfs(u, edge, states_masks);

    for (size_t u = 0; u < edge.size(); u++)
        if (is_discovered(u, states_masks))
            cout << u << endl;
}

void reverse_productive_dfs(int u, const vector<vector<int>> &edge,
                            vector<uint8_t> &states_masks,
                            vector<list<int>> &parents)
{
    for (auto v : parents[u]) {
        if (!is_productive(v, states_masks)) {
            states_masks[v] |= PRODUCTIVE_FLAG;
            reverse_productive_dfs(v, edge, states_masks, parents);
        }
    }
}

void reverse_productive_bfs(int u, const vector<vector<int>> &edge,
                            vector<uint8_t> &states_masks,
                            vector<list<int>> &parents)
{
    list<int> queue = {u};
    while (!queue.empty()) {
        int v = queue.front();
        queue.pop_front();

        for (auto w : parents[v]) {
            if (!is_productive(w, states_masks)) {
                states_masks[w] |= PRODUCTIVE_FLAG;
                queue.push_back(w);
            }
        }
    }
}

void productive_dfs(int u, const vector<vector<int>> &edge,
                    vector<uint8_t> &states_masks,
                    vector<list<int>> parents)
{
    list<int> stack = {u};
    states_masks[u] |= DISCOVERED_FLAG;

    while (!stack.empty()) {
        int v = stack.front();
        stack.pop_front();

        if (is_final_state(v, states_masks)) {
            // o stare finala e automat productiva
            states_masks[v] |= PRODUCTIVE_FLAG;
        }

        if (is_productive(v, states_masks)) {
            // reverse_productive_dfs(v, edge, states_masks, parents);
            reverse_productive_bfs(v, edge, states_masks, parents);
        }

        for (size_t c = 0; c < edge[v].size(); c++) {
            int w = edge[v][c];

            if (!is_discovered(w, states_masks)) {
                states_masks[w] |= DISCOVERED_FLAG;
                stack.push_front(w);
            }
        }

        states_masks[v] |= VISITED;
    }
}

void find_productive_states(const vector<vector<int>> &edge,
                            vector<uint8_t> &states_masks,
                            const vector<list<int>> &parents)
{
    for (size_t u = 0; u < edge.size(); u++)
        if (!is_discovered(u, states_masks))
            productive_dfs(u, edge, states_masks, parents);

    for (size_t u = 0; u < edge.size(); u++)
        if (is_productive(u, states_masks))
            cout << u << endl;
}

void find_useful_states(vector<vector<int>> &edge,
                      vector<int> &start_states,
                      vector<uint8_t> &states_masks,
                      const vector<list<int>> &parents)
{
    for (int u : start_states)
        if (!is_discovered(u, states_masks))
            productive_dfs(u, edge, states_masks, parents);

    for (size_t u = 0; u < edge.size(); u++)
        if (is_discovered(u, states_masks) && is_productive(u, states_masks))
            cout << u << endl;
}

void read_input(const string &problem, int n, int m, int s, int f,
                vector<vector<int>> &edge, vector<list<int>> &parents,
                vector<int> &start_states, vector<uint8_t> &states_mask)
{
    if (problem == "productive" || problem == "useful") {
        parents = vector<list<int>>(n);

        int next_state;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c < m; c++) {
                cin >> next_state;
                edge[i][c] = next_state;

                if ((parents[next_state].empty()
                                || parents[next_state].front() != i)
                            && next_state != i)
                    parents[next_state].push_front(i);
            }
        }
    } else {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                cin >> edge[i][j];
    }

    if (s > 0) {
        start_states = vector<int>(s);
        for (int i = 0; i < s; i++)
            cin >> start_states[i];
    }

    if (f > 0) {
        for (int i = 0; i < f; i++) {
            int state;
            cin >> state;
            states_mask[state] |= FINAL_STATE_FLAG;
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        cout << "Usage: ./main <problem>" << endl;
        return 0;
    }
    string problem(argv[1]);
    int n, m, s, f;
    cin >> n >> m >> s >> f;

    // edge[0][map('a')] = urmatorul nod din starea 0 pe simbolul a;
    // ca memorie, sunt n vectori de cate m elemente capacitate fiecare
    // ce urmeaza sa fie initializate
    vector<vector<int>> edge(n, vector<int>(m));

    // necesar pentru cautarea in adancime la productive si useful
    vector<list<int>> parents;

    vector<int> start_states;
    vector<uint8_t> states_mask(n, 0);

    read_input(problem, n, m, s, f, edge, parents, start_states, states_mask);

    if (problem == "accessible")
        find_accessible_states(edge, start_states, states_mask);
    else if (problem == "productive")
        find_productive_states(edge, states_mask, parents);
    else if (problem == "useful")
        find_useful_states(edge, start_states, states_mask, parents);
    else if (problem == "synchronize")
        find_syncronize_sequence_trivial(edge);
    return 0;
}
