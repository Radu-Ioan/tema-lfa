#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <list>

#include "synchronize.h"
#include "flags.h"

using namespace std;

static void bfs(list<pair<pair<int, int>, vector<int> *>> &bfs_q,
            vector<vector<bool>> &visited, const vector<vector<int>> &edge,
            vector<int> &y, vector<int> *empty_vec_address)
{
    int m = edge[0].size();

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
}

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

void find_syncronize_sequence_trivial(const vector<vector<int>> &edge,
                    const vector<int> &start_states,
                    vector<uint8_t> &states_masks)
{
    int n = edge.size();
    vector<int> y = {};

    if (start_states.size() > 0) {
        for (auto state : start_states)
            states_masks[state] |= ACTIVE_FLAG;
    } else {
        for (size_t i = 0; i < edge.size(); i++)
            states_masks[i] |= ACTIVE_FLAG;
    }

    vector<vector<bool>> visited(n, vector<bool>(n, false));
    list<pair<pair<int, int>, vector<int> *>> bfs_q;
    vector<int> empty_vec = {};

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
        bfs(bfs_q, visited, edge, y, &empty_vec);

        for (int a : y)
            cout << a << ' ';
    }
    cout << endl;
}
