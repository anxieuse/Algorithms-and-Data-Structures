#include <bits/stdc++.h>

using namespace std;

using llong = long long;
using ldouble = long double;

const int INT_INF = INT_MAX / 3;
const ldouble DBL_INF = DBL_MAX / 3;

struct Point
{
    int x, y;
    friend istream &operator>>(istream &in, Point &pt);
};

istream &operator>>(istream &in, Point &pt)
{
    in >> pt.x >> pt.y;
    return in;
}

class Graph
{
public:
    Graph() = default;

    Graph(int n, int m) : n(n), m(m)
    {
        adj_list.resize(n);
        pt.resize(n);
    }

    void read_coords()
    {
        for (int i = 0; i < n; ++i)
        {
            cin >> pt[i];
        }
    }

    void read_edges()
    {
        int from, to;
        for (int i = 0; i < m; ++i)
        {
            cin >> from >> to;
            --from, --to;
            adj_list[from].emplace_back(to);
            adj_list[to].emplace_back(from);
        }
    }

    void read_graph()
    {
        read_coords();
        read_edges();
    }

    ldouble calc_dist(int u, int v)
    {
        return hypot<ldouble>(pt[u].x - pt[v].x, pt[u].y - pt[v].y);
    }

    ldouble calc_shortest_dist(int start, int dest)
    {
        // par.resize(n, -1);

        vector<ldouble> f(n, DBL_INF);

        set<pair<ldouble, int>> st;
        f[start] = 0;
        st.insert({f[start], start});

        while (not st.empty())
        {
            int cur = (*st.begin()).second;
            st.erase(st.begin());
            for (auto to : adj_list[cur])
            {
                if (f[to] > f[cur] + calc_dist(cur, to))
                {
                    st.erase({f[to], to});
                    f[to] = f[cur] + calc_dist(cur, to);
                    // par[to] = cur;
                    st.insert({f[to], to});
                }
            }
        }

        // ldouble shortest_dist = 0;
        // for (int cur = dest; cur != start; cur = par[cur])
        // {
            // shortest_dist += calc_dist(cur, par[cur]);
                // cerr << cur + 1 << " <--(" << calc_dist(cur, par[cur]) << ")-- ";
        // }
        // cerr << start + 1 << '\n';

        // assert(fabs(shortest_dist - f[dest]) <= 1e-7);
        // cerr << shortest_dist << '\n';

        return f[dest];
    }

private:
    int n, m;
    vector<vector<int>> adj_list;
    vector<Point> pt;
    // vector<int> par;
};

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);
    cout << fixed << setprecision(6);

    int n, m;
    cin >> n >> m;

    Graph gr(n, m);
    gr.read_graph();

    int q;
    cin >> q;
    for (int i = 0; i < q; ++i)
    {
        int start, dest;
        cin >> start >> dest;
        --start, --dest;
        ldouble distance = gr.calc_shortest_dist(start, dest);
        cout << distance << '\n';
    }

    return 0;
}
