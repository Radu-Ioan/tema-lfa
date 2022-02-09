#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void find_accessible_states(vector<vector<int>> &edge,
                            vector<int> &start_states,
                            vector<int> &final_states)
{

}

void find_productive_states(vector<vector<int>> &edge,
                            vector<int> &start_states,
                            vector<int> &final_states)
{

}

void find_util_states(vector<vector<int>> &edge,
                      vector<int> &start_states,
                      vector<int> &final_states)
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
    vector<int> final_states;

    if (s > 0)
        start_states = vector<int>(s);
    if (f > 0)
        final_states = vector<int>(f);

    for (int i = 0; i < s; i++)
        cin >> start_states[i];
    for (int i = 0; i < f; i++)
        cin >> final_states[i];

    if (problem == "accessible")
        find_accessible_states(edge, start_states, final_states);
    else if (problem == "productive")
        find_productive_states(edge, start_states, final_states);
    else if (problem == "utils")
        find_util_states(edge, start_states, final_states);
    return 0;
}
