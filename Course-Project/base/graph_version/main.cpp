#include <cfloat>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits.h>
#include <queue>
#include <vector>

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

    inline ldouble calc_dist(int u, int v)
    {
        return hypot<ldouble>(pt[u].x - pt[v].x, pt[u].y - pt[v].y);
    }

    ldouble calc_shortest_dist(int start, int dest)
    {
        vector<ldouble> d(n, DBL_INF);
        d[start] = 0;

        priority_queue<pair<ldouble, int>, vector<pair<ldouble, int>>, greater<pair<ldouble, int>>> q;
        q.push({calc_dist(start, dest), start});

        vector<bool> closed(n, false);

        while (not q.empty())
        {
            pair<ldouble, int> cur = q.top();
            q.pop();

            closed[cur.second] = true;

            if (cur.second == dest)
            {
                break;
            }

            for (auto to : adj_list[cur.second])
            {
                if (!closed[to] and d[to] > d[cur.second] + calc_dist(cur.second, to))
                {
                    d[to] = d[cur.second] + calc_dist(cur.second, to);
                    q.push({d[to] + calc_dist(to, dest), to});
                }
            }
        }

        return d[dest];
    }

private:
    int n, m;
    vector<vector<int>> adj_list;
    vector<Point> pt;
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
