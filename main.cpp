#include <iostream>
#include <fstream>
#include <vector>
#include <list>

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

void find_syncronize_sequence_trivial(const vector<vector<int>> &edge)
{
    int n = edge.size();
    int m = edge[0].size();

    // automatul produs: contine toate submultimile de cate 2 elemente,
    // prod_autom_edges[i][j][c] = urmatoarea stare din starea produs (i, j)
    // primind simbolul c
    vector<vector<vector<pair<int, int>>>>
                prod_autom_edges(n, vector<vector<pair<int, int>>>(
                                            n, vector<pair<int, int>>(m)));

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            for (int c = 0; c < m; c++) {
                int next_state_for_i = edge[i][c];
                int next_state_for_j = edge[j][c];

                prod_autom_edges[i][j][c] = prod_autom_edges[j][i][c]
                    = {next_state_for_i, next_state_for_j};
            }
        }
    }

    vector<int> x = {};
    vector<int> y = {};

    vector<bool> is_active_state(n, true);
    vector<bool> is_destination_state(n, false);

    int active_states = 0;

    vector<vector<bool>> visited(n, vector<bool>(n, false));

    while (true) {
        active_states = 0;

        for (int k = 0; k < n; k++) {
            if (!is_active_state[k])
                continue;

            int state = k;
            for (size_t i = 0; i < y.size(); i++)
                state = edge[state][y[i]];

            if (!is_destination_state[state]) {
                is_destination_state[state] = true;
                ++active_states;
            }
        }

        int s0, t0;
        int i;
        for (i = 0; i < n; i++) {
            if (is_active_state[i]) {
                s0 = i;
                break;
            }
        }
        for (i = i + 1; i < n; i++) {
            if (is_active_state[i]) {
                t0 = i;
                break;
            }
        }

        list<pair<pair<int, int>, vector<int>>>
                q = {{{s0, t0}, {}}};

        // aici se refac visited si y
        for (i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                visited[i][j] = false;
        y.clear();

        // bfs ul in automatul produs ca sa gasesc merging sequence
        while (!q.empty()) {
            auto [state_pair, str_to_here] = q.front();
            auto [state_i, state_j] = state_pair;
            visited[state_i][state_j] = true;
            q.pop_front();

            for (auto c = 0; c < m; c++) {
                auto [next_state_i, next_state_j]
                    = prod_autom_edges[state_i][state_j][c];

                if (visited[next_state_i][next_state_j])
                    continue;

                if (next_state_i == next_state_j) {
                    y = str_to_here;
                    y.emplace_back(c);
                    q.clear();
                    break;
                } else {
                    str_to_here.emplace_back(c);
                    q.push_back({{next_state_i, next_state_j}, str_to_here});
                }
            }
        }

        for (int a : y)
            x.emplace_back(a);

        if (active_states <= 1)
            break;

        for (i = 0; i < n; i++) {
            is_active_state[i] = is_destination_state[i];
            is_destination_state[i] = false;
        }
    }

    for (size_t i = 0; i < x.size(); i++)
        cout << x[i] << ' ';
    cout << endl;
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

    // necesar pentru cautarea in adancime la productive si useful
    vector<list<int>> parents;

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

    vector<int> start_states;
    vector<uint8_t> states_mask(n, 0);

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
