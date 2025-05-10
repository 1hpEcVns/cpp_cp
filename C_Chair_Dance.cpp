// 数论 线性同余方程 逆元
// 本代码没有使用桶排序 性能较低
// 每次去掉同余产生的重复元素后 至少减半
// 所以时间复杂度为 O(nlogn)
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

using namespace std;

auto main() -> int {
  ios::sync_with_stdio(false), cin.tie(nullptr);
  int n, q, o, add{}, mul{1};
  cin >> n >> q;
  const int N = n;
  auto a = vector(n, array{0, 0, 0});
  for (auto &&[i, t] : views::enumerate(a))
    t[0] = t[2] = i;
  while (q--) {
    char op;
    cin >> op >> o;
    if (op == '+') {
      add = (add + o) % N;
      continue;
    }
    if (op == '*') {
      int g = gcd(n, o);
      if (g == 1) {
        add = 1ll * add * o % N, mul = 1ll * mul * o % n;
        continue;
      }
      for (auto &[x, y, z] : a) {
        int t = (1ll * x * mul % n * (N / n) + add) % N;
        y = (1ll * t * o - t + N) % N, x %= n / g;
      }
      n /= g, add = 1ll * add * o % N, mul = 1ll * mul * (o / g) % n;
      ranges::sort(a);
      a.resize(ranges::unique(
                   a, [](const auto &a, const auto &b) { return a[0] == b[0]; })
                   .begin() -
               a.begin());
      continue;
    }
    o = (o - add + N) % N;
    if (o % (N / n)) {
      println("-1");
      continue;
    }
    o /= N / n;
    int x, y;
    auto const ex = [&x, &y](this auto ex, int u, int v) {
      if (v == 0) {
        x = 1, y = 0;
        return;
      }
      ex(v, u % v);
      tie(x, y) = pair(y, x - u / v * y);
    };
    ex(n, mul);
    o = (1ll * y * o % n + n) % n;
    auto const it = ranges::lower_bound(a, array{o, 0, 0});
    println("{}", (it == a.end() || it->front() != o) ? -1
                  : it->back()                        ? it->back()
                                                      : N);
  }
}