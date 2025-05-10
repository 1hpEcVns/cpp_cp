//这是一个可持久化动态树问题 存在低于O(n^2)的解法
// 但是本题数据范围较小 所以可以暴力
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <unordered_map>
#include <vector>

using namespace std;

auto main() -> int {
  auto n = 0, m = 0;
  cin >> n >> m;
  auto a = vector<int>(n);
  auto b = vector<vector<int>>{{}};
  auto c = unordered_map<int, unordered_map<int, int>>{};
  while (m--) {
    auto op = '?';
    auto x = 0, y = 0;
    cin >> op >> x >> y;
    auto &u = a[x - 1], &v = a[y - 1];
    auto cmp = [&](this auto &cmp, const int x, const int y) {
      auto &p = c[x];
      auto t = p.find(y);
      if (t != p.end())
        return t->second;
      for (auto const &[i, j] : views::zip(b[x], b[y]))
        if (cmp(i, j))
          return p.emplace(y, cmp(i, j)).first->second;
      return p
          .emplace(y, b[x].size() != b[y].size()
                          ? b[x].size() < b[y].size() ? -1 : 1
                          : 0)
          .first->second;
    };
    if (op == '?') {
      println("{}", cmp(u, v) ? cmp(u, v) < 0 ? '<' : '>' : '=');
      continue;
    }
    b.push_back(b[u]), u = b.size() - 1;
    b[u].insert(
        ranges::lower_bound(
            b[u], v, [&](const int x, const int y) { return cmp(x, y) > 0; }),
        v);
  }
}