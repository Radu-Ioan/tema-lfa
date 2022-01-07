#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void find_accessible_states(vector<vector<int>> &edge,
                            vector<int> &start_states,
                            vector<int> &final_states,
                            ofstream &out)
{

}

void find_productive_states(vector<vector<int>> &edge,
                            vector<int> &start_states,
                            vector<int> &final_states,
                            ofstream &out)
{

}

void find_util_states(vector<vector<int>> &edge,
                      vector<int> &start_states,
                      vector<int> &final_states,
                      ofstream &out)
{
    
}


int main(int argc, char const *argv[])
{
    if (argc < 2) {
        cout << "Usage: ./main <problem>" << endl;
        return 0;
    }

    string problem(argv[1]);
    cout << "problem: " << problem << endl;

    ifstream in_stream(problem + ".in");
    ofstream out_stream(problem + ".out");

    int n, m, s, f;
    in_stream >> n >> m >> s >> f;

    // edge[0][map('a')] = urmatorul nod din starea 0 pe simbolul a;
    // ca memorie, sunt n vectori de cate m elemente capacitate fiecare
    // urmeaza sa fie initializate
    vector<vector<int>> edge(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            in_stream >> edge[i][j];

    vector<int> start_states;
    vector<int> final_states;

    if (s > 0)
        start_states = vector<int>(s);
    if (f > 0)
        final_states = vector<int>(f);

    for (int i = 0; i < s; i++)
        in_stream >> start_states[i];
    for (int i = 0; i < f; i++)
        in_stream >> final_states[i];

    if (problem == "accessible")
        find_accessible_states(edge, start_states, final_states, out_stream);
    else if (problem == "productive")
        find_productive_states(edge, start_states, final_states, out_stream);
    else if (problem == "utils")
        find_util_states(edge, start_states, final_states, out_stream);

    in_stream.close();
    out_stream.close();
    return 0;
}
