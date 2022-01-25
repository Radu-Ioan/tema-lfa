#include <iostream>
#include <vector>
#include <list>
#include <utility>

#include "synchronize.h"
#include "flags.h"

using namespace std;


static void depart_from_active_states(const vector<vector<int>> &edge,
                    vector<uint8_t> &states_masks, int &active_states,
                    const vector<int> &y, int &s0, int &t0)
{
    int n = edge.size();

    for (int k = 0; k < n; k++) {
        if (!(states_masks[k] & ACTIVE_FLAG))
            continue;

        int state = k;
        for (size_t i = 0; i < y.size(); i++)
            state = edge[state][y[i]];

        if (!(states_masks[state] & DESTINATION_FLAG)) {
            states_masks[state] |= DESTINATION_FLAG;
            ++active_states;

            if (s0 == -1)
                s0 = state;
            else if (t0 == -1)
                t0 = state;
        }
    }
}

static void set_the_new_active_states(vector<uint8_t> &states_masks)
{
    int n = states_masks.size();
    for (int i = 0; i < n; i++) {
        (states_masks[i] & DESTINATION_FLAG)
            ? states_masks[i] |= ACTIVE_FLAG
            : states_masks[i] &= ~ACTIVE_FLAG;

        states_masks[i] &= ~DESTINATION_FLAG;
    }
}

static int bfs(list<pair<pair<int, int>, vector<int> *>> &bfs_q,
            vector<vector<bool>> &visited, const vector<vector<int>> &edge,
            vector<int> &y, vector<int> *empty_vec_address)
{
    int m = edge[0].size();
    int singleton_state = -1;

    while (!bfs_q.empty()) {
        auto [state_pair, str_to_here] = bfs_q.front();
        auto [i, j] = state_pair;
        visited[i][j] = visited[j][i] = true;
        bfs_q.pop_front();

        for (int c = 0; c < m; c++) {
            int next_i = edge[i][c];
            int next_j = edge[j][c];

            if (visited[next_i][next_j])
                continue;

            if (next_i == next_j) {
                y = *str_to_here;
                y.emplace_back(c);
                singleton_state = next_i;

                while (!bfs_q.empty()) {
                    auto &[some_pair, some_str] = bfs_q.front();
                    delete some_str;
                    bfs_q.pop_front();
                }

                break;
            } else {
                auto str_to_expand
                    = new vector<int>((*str_to_here).size() + 1);

                // pun la final sirul cu care s-a ajuns la starea care s-a
                // scos din coada la care concatenez sirul in care se
                // ajunge starea nevizitata
                size_t k = 0;
                while (k < (*str_to_here).size()) {
                    (*str_to_expand)[k] = (*str_to_here)[k];
                    k++;
                }
                (*str_to_expand)[k] = c;

                bfs_q.push_back({{next_i, next_j}, str_to_expand});
                visited[next_i][next_j] = visited[next_j][next_i] = true;
            }
        }

        if (str_to_here != empty_vec_address)
            delete str_to_here;
    }

    return singleton_state;
}

/* asigura ca starea in care duce sirul de sincronizare este marcata ca finala
 */
static void go_until_a_final_state(int state,
                        const vector<vector<int>> &edge,
                        vector<uint8_t> &states_masks)
{
    if (states_masks[state] & FINAL_STATE_FLAG)
        return;

    // starea de prelucrat, parintele din care s-a ajuns in stare, simbolul pe
    // care s-a ajuns in stare
    list<int> q = {state};

    int n = edge.size();
    int m = edge[0].size();

    for (int i = 0; i < n; i++)
        states_masks[i] &= ~VISITED;
    states_masks[state] |= VISITED;

    vector<int> parent(n, -1);
    vector<int> symbol(n, -1);
    vector<int> extra_path{};

    while (!q.empty()) {
        auto k = q.front();
        q.pop_front();

        for (int i = 0; i < m; i++) {
            int next = edge[k][i];

            if (states_masks[next] & FINAL_STATE_FLAG) {
                extra_path.emplace_back(i);
                for (int j = parent[next]; j != -1; j = parent[j])
                    extra_path.emplace_back(symbol[j]);
                q.clear();
                break;
            } else if (!(states_masks[next] & VISITED)) {
                q.push_back(next);
                parent[next] = k;
                symbol[next] = i;
                states_masks[next] |= VISITED;
            }
        }
    }

    for (auto it = extra_path.rbegin(); it != extra_path.rend(); ++it)
        cout << *it << ' ';
}

void find_syncronize_sequence_trivial(const vector<vector<int>> &edge,
                    const vector<int> &start_states,
                    vector<uint8_t> &states_masks)
{
    int n = edge.size();
    // variabila in care se pune cate un merging sequence pentru 2 stari active
    // alese la un pas
    vector<int> y = {};

    if (start_states.size() > 0) {
        // pentru sincronizarea partiala se porneste cautarea unui sir de
        // sincronizare doar pentru anumite stari
        for (auto state : start_states)
            states_masks[state] |= ACTIVE_FLAG;
    } else {
        // pe cazul general, toate starile sunt initiale, deci active
        for (size_t i = 0; i < edge.size(); i++)
            states_masks[i] |= ACTIVE_FLAG;
    }

    vector<vector<bool>> visited(n, vector<bool>(n, false));
    list<pair<pair<int, int>, vector<int> *>> bfs_q;
    vector<int> empty_vec = {};
    int singleton_state;

    while (true) {
        int active_states = 0;
        int s0 = -1, t0 = -1;
        depart_from_active_states(edge, states_masks, active_states,
                                    y, s0, t0);
        if (active_states <= 1)
            break;

        set_the_new_active_states(states_masks);
        y.clear();

        // pregatesc bfs
        bfs_q.push_front({{s0, t0}, &empty_vec});
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                visited[i][j] = false;

        // caut un merging sequence
        singleton_state = bfs(bfs_q, visited, edge, y, &empty_vec);

        for (int a : y)
            cout << a << ' ';
    }

    go_until_a_final_state(singleton_state, edge, states_masks);
    cout << endl;
}
