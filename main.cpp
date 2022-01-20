#include <iostream>
#include <fstream>
#include <vector>
#include <list>

using namespace std;

#define VISITED_FLAG (uint8_t) 1
#define FINAL_STATE_FLAG (uint8_t) 2
#define PRODUCTIVE_FLAG (uint8_t) 4

inline bool is_visited(int u, const vector<uint8_t> &masks)
{
    return masks[u] & VISITED_FLAG;
}

inline bool is_final_state(int u, const vector<uint8_t> &masks)
{
    return masks[u] & FINAL_STATE_FLAG;
}

inline bool is_productive(int u, const vector<uint8_t> &masks)
{
    return masks[u] & PRODUCTIVE_FLAG;
}

void accessible_dfs(int u, const vector<vector<int>> &edge,
                    vector<bool> &is_accessible)
{
    list<int> stack = {u};

    while (!stack.empty()) {
        int v = stack.front();
        stack.pop_front();
        is_accessible[v] = true;

        for (size_t c = 0; c < edge[v].size(); c++) {
            int w = edge[v][c];

            if (!is_accessible[w]) {
                stack.push_front(w);
            }
        }
    }
}

void find_accessible_states(vector<vector<int>> &edge,
                            vector<int> &start_states)
{
    vector<bool> is_accessible(edge.size(), false);

    for (int u : start_states)
        if (!is_accessible[u])
            accessible_dfs(u, edge, is_accessible);

    for (size_t u = 0; u < edge.size(); u++)
        if (is_accessible[u])
            cout << u << endl;
}

void productive_dfs(int u, const vector<vector<int>> &edge,
                    vector<uint8_t> &states_masks,
                    vector<int> &p)
{
    list<int> stack = {u};

    while (!stack.empty()) {
        int v = stack.front();
        stack.pop_front();
        states_masks[v] |= VISITED_FLAG;

        if (is_final_state(v, states_masks)) {
            // o stare finala e automat productiva
            states_masks[v] |= PRODUCTIVE_FLAG;

            int prev = p[v];

            while (prev != -1) {
                if (is_productive(prev, states_masks)) {
                    // nodurile din calea pana la o radacina deja au fost
                    // setate ca productive la o alta parcurgere
                    break;
                } else {
                    states_masks[prev] |= PRODUCTIVE_FLAG;
                    prev = p[prev];
                }
            }
        }

        for (size_t c = 0; c < edge[v].size(); c++) {
            int w = edge[v][c];

            // sau
            // if (p[w] == -1)
            if (!is_visited(w, states_masks)) {
                p[w] = v;
                stack.push_front(w);
            }
        }
    }
}

void find_productive_states(const vector<vector<int>> &edge,
                            vector<uint8_t> &states_masks)
{
    vector<int> p(edge.size(), -1);
    // incearca mai bine ceva cu
    list<int> parents(edge.size());

    for (size_t u = 0; u < edge.size(); u++)
        if (!is_visited(u, states_masks))
            productive_dfs(u, edge, states_masks, p);

    for (size_t u = 0; u < edge.size(); u++)
        if (is_productive(u, states_masks))
            cout << u << endl;
}

void find_util_states(vector<vector<int>> &edge,
                      vector<int> &start_states,
                      vector<uint8_t> &states_masks)
{
    
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
    // urmeaza sa fie initializate
    vector<vector<int>> edge(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            cin >> edge[i][j];

    vector<int> start_states;
    vector<uint8_t> states_mask;

    if (s > 0)
        start_states = vector<int>(s);
    if (f > 0)
        states_mask = vector<uint8_t>(n, 0);

    for (int i = 0; i < s; i++)
        cin >> start_states[i];
    for (int i = 0; i < f; i++) {
        int state;
        cin >> state;
        states_mask[state] |= FINAL_STATE_FLAG;
    }

    if (problem == "accessible")
        find_accessible_states(edge, start_states);
    else if (problem == "productive")
        find_productive_states(edge, states_mask);
    else if (problem == "utils")
        find_util_states(edge, start_states, states_mask);
    return 0;
}
