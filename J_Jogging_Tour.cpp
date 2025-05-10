#include <array>
#include <cmath>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

#define ts views::transform

using namespace std;
using namespace views;
using f64 = double;

auto main() -> int {
  ios::sync_with_stdio(false), cin.tie(nullptr);
  int n;
  f64 ans = 1e18;
  cin >> n;
  auto a = vector(n, array{0, 0});
  for (auto &&[x, y] : a)
    cin >> x >> y;
  for (auto &&[i, A] : enumerate(a))
    for (auto &[x2, y2] : a | drop(i + 1)) {
      auto [x1, y1] = A;
      f64 x = x1 - x2, y = y1 - y2, r = sqrt(x * x + y * y), s = -y / r,
          c = x / r; // 逆时针旋转
      auto b = a | ts([=](auto &p) {
                 auto [x, y] = p;
                 return array{x * c - y * s, x * s + y * c};
               }) |
               ranges::to<vector>();
      auto g = b | ts([&](const auto &x) {
                 return b | ts([&](const auto &y) {
                          return abs(x[0] - y[0]) + abs(x[1] - y[1]);
                        }) |
                        ranges::to<vector>();
               }) |
               ranges::to<vector>();
      auto f = vector(1 << n, vector(n, 1e18));
      for (int i : iota(0, n))
        f[1 << i][i] = 0;
      for (auto &&[i, F] : enumerate(f) | drop(1))
        for (int k : iota(0, n) | filter([&](int k) { return !(i >> k & 1); }))
          f[i | (1 << k)][k] = ranges::min(
              iota(0, n) | filter([&](int j) { return i >> j & 1; }) |
              ts([&](int j) { return F[j] + g[k][j]; }));
      ans = min(ans, ranges::min(f.back()));
    }
  print("{:.9f}", ans);
}