#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <list>

#include "synchronize.h"

using namespace std;


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

    vector<int> y = {};

    vector<bool> is_active_state(n, true);
    vector<bool> is_destination_state(n, false);
    int active_states = 0;

    vector<vector<bool>> visited(n, vector<bool>(n, false));
    // list<pair<pair<int, int>, vector<int>>> bfs_q;
    list<pair<pair<int, int>, vector<int> *>> bfs_q;
    vector<int> empty_vec = {};


    // ramane sa vezi ce se intampla la mid1-s.in
    ofstream otup("debug.txt");

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

        otup << active_states << endl;

        if (active_states <= 1)
            break;

        for (int i = 0; i < n; i++) {
            is_active_state[i] = is_destination_state[i];
            is_destination_state[i] = false;
        }

        y.clear();

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


        bfs_q = {{{s0, t0}, &empty_vec}};
        for (i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                visited[i][j] = false;

        // bfs ul in automatul produs ca sa gasesc merging sequence
        while (!bfs_q.empty()) {
            auto [state_pair, str_to_here] = bfs_q.front();
            auto [state_i, state_j] = state_pair;
            visited[state_i][state_j] = visited[state_j][state_i] = true;
            bfs_q.pop_front();

            for (auto c = 0; c < m; c++) {
                auto [next_state_i, next_state_j]
                    = prod_autom_edges[state_i][state_j][c];

                if (visited[next_state_i][next_state_j])
                    continue;

                if (next_state_i == next_state_j) {
                    y = *str_to_here;
                    if (y.size() > 0)
                        delete str_to_here;
                    y.emplace_back(c);

                    while (!bfs_q.empty()) {
                        auto &[some_pair, some_str] = bfs_q.front();
                        delete some_str;
                        bfs_q.pop_front();
                    }

                    break;
                } else {
                    auto appended_str = new vector<int>();
                    *appended_str = *str_to_here;
                    (*appended_str).emplace_back(c);
                    bfs_q.push_back({{next_state_i, next_state_j},
                                        appended_str});
                    // ia sa incerci si cu asta
                    visited[state_i][state_j] = visited[state_j][state_i]
                        = true;
                }
            }
        }

        for (int a : y)
            cout << a << ' ';
    }

    otup.close();

    cout << endl;
}

