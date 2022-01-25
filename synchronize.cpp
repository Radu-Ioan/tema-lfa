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

    const uint8_t ACTIVE_FLAG = 1;
    const uint8_t DESTINATION_FLAG = 2;
    vector<uint8_t> states_masks(n, ACTIVE_FLAG);

    int active_states = 0;

    vector<vector<bool>> visited(n, vector<bool>(n, false));
    list<pair<pair<int, int>, vector<int> *>> bfs_q;
    vector<int> empty_vec = {};

    ofstream otup("debug.txt");

    while (true) {
        active_states = 0;

        int s0 = -1, t0 = -1;

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

        otup << active_states << endl;

        if (active_states <= 1)
            break;

        for (int i = 0; i < n; i++) {
            (states_masks[i] & DESTINATION_FLAG)
                ? states_masks[i] |= ACTIVE_FLAG
                : states_masks[i] &= ~ACTIVE_FLAG;

            states_masks[i] &= ~DESTINATION_FLAG;
        }

        y.clear();

        bfs_q.push_front({{s0, t0}, &empty_vec});

        for (int i = 0; i < n; i++)
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
                    y.emplace_back(c);

                    while (!bfs_q.empty()) {
                        auto &[some_pair, some_str] = bfs_q.front();
                        delete some_str;
                        bfs_q.pop_front();
                    }

                    break;
                } else {
                    auto appended_str
                        = new vector<int>((*str_to_here).size() + 1);

                    // pun la final sirul cu care s-a ajuns la starea care s-a
                    // scos din coada la care concatenez sirul in care se
                    // ajunge starea nevizitata
                    size_t i = 0;
                    while (i < (*str_to_here).size()) {
                        (*appended_str)[i] = (*str_to_here)[i];
                        i++;
                    }
                    (*appended_str)[i] = c;

                    bfs_q.push_back({{next_state_i, next_state_j},
                                        appended_str});
                    visited[next_state_i][next_state_j]
                        = visited[next_state_j][next_state_i] = true;
                }
            }

            if (str_to_here != &empty_vec)
                delete str_to_here;
        }

        for (int a : y)
            cout << a << ' ';
    }

    otup.close();

    cout << endl;
}

