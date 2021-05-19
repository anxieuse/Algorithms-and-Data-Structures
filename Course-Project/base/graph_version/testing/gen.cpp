#include <bits/stdc++.h>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
using distrib_t = uniform_int_distribution<int>;

inline int rand(int a, int b)
{
    return distrib_t(a, b)(rng);
}

struct Edge
{
    int from, to;
};

bool operator<(const Edge &lhs, const Edge &rhs)
{
    if (lhs.from == rhs.from)
    {
        return lhs.to < rhs.to;
    }
    return lhs.from < rhs.from;
}

int main(int argc, char **argv)
{
    ios::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);
    cout << fixed << setprecision(6);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    vector<pair<int, int>> coords(n);
    map<pair<int, int>, bool> coords_exist;
    int max_coordinate = n;
    for (int i = 0; i < n; ++i)
    {
        do
        {
            coords[i].first = rand(0, max_coordinate - 1);
            coords[i].second = rand(coords[i].first, max_coordinate);
        } while (coords_exist[coords[i]] == true);
        coords_exist[coords[i]] = true;
    }

    set<Edge> st;
    
    int cur_v = 1;
    while (cur_v + 1 <= n)
    {
        int from = cur_v + 1;
        int to = rand(1, cur_v);
        st.insert({to, from});
        cur_v++;
    }

    while (st.size() < m)
    {
        int from, to;
        do
        {
            from = rand(1, n - 1);
            to = rand(from + 1, n);
        } while (st.count({from, to}) > 0 or from == to);
        st.insert({from, to});
    }

    cout << n << ' ' << m << '\n';

    cout << '\n';

    for (int i = 0; i < n; ++i)
    {
        cout << coords[i].first << ' ' << coords[i].second << '\n';
    }

    cout << '\n';

    for (auto edge : st)
    {
        cout << edge.from << ' ' << edge.to << '\n';
    }

    cout << '\n';

    int q = 1;
    cout << q << '\n';

    for (int i = 0; i < q; ++i) {
        int start = 1;
        int dest = n;
        cout << start << ' ' << dest << '\n';
    }

    return 0;
}
