// 这是一道最短路问题
// 需要使用堆优化Dijkstra算法
// 使用优先队列实现
// 注意建图要开根号
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <print>
#include <queue>
#include <ranges>
#include <vector>

using namespace std;

auto main() -> int {
  ios::sync_with_stdio(false), cin.tie(nullptr);
  int n, m, q;
  cin >> n >> m >> q;
  auto const &g = [n, m]() {
    auto a = vector<array<int, 3>>(n);
    auto b = vector<int>(n);
    auto G = vector<array<int, 2>>(m);
    for (auto &[x, y, z] : a)
      cin >> x >> y >> z;
    for (auto &[u, v] : G)
      cin >> u >> v, u--, v--, ++b[u], ++b[v];
    auto g = vector<vector<pair<double, int>>>(n);
    for (auto &&[n, g] : views::zip(b, g))
      g.reserve(n);
    for (auto const &[u, v] : G) {
      auto const &[x1, y1, z1] = a[u];
      auto const &[x2, y2, z2] = a[v];
      auto const d = 2 * sqrt(hypot(x1 - x2, y1 - y2, z1 - z2));
      g[u].emplace_back(d, v), g[v].emplace_back(d, u);
    }
    return g;
  }();//这里的lambda表达式立即调用构造可以减少内存管理开销和命名冲突
  auto Q = [m]() {
    auto Q_mem = vector<pair<double, int>>();
    return Q_mem.reserve(m), priority_queue(greater{}, std::move(Q_mem));
  }();
  auto f = vector<double>(n, 1e9);
  auto h = [&f, &Q](const pair<double, int> &a) {
    if (a.first < f[a.second])
      f[a.second] = a.first, Q.emplace(a);
  };
  for (h({0, 0}); !Q.empty();)
    if (auto const [d, u] = Q.top(); Q.pop(), f[u] == d)
      for (auto const &[w, v] : g[u])
        h({d + w, v});
  for (int u, t; q--;) {
    cin >> u >> t, u--;
    auto const d = t * t - f[u] * f[u];
    d < 0 ? println("impossible") : println("{:.10f}", t - sqrt(d));
  }
}