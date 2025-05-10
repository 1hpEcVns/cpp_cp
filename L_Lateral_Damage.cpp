#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

using namespace std;

int main() {
  int n, k;
  std::string s;
  cin >> n >> k;
  auto v_mem = std::vector<bool>(n * n);
  auto v = [n, &v_mem](int x, int y) { return v_mem[(x - 1) * n + (y - 1)]; };
  auto out = [&](int x, int y) { println("{} {}", x, y), cin >> s; };
  for (int i : views::iota(1, n + 1))
    for (int j : views::iota(1, n + 1))
      if (i % 5 == j % 5 && (out(i, j), s == "hit"))
        v(i, j) = true;
  auto ck = [&](int x, int y) {
    auto ckx = [&](int x) { return clamp(x, 1, n) == x; };
    if (ckx(x) && ckx(y))
      out(x, y);
  };
  for (int i : views::iota(1, n + 1))
    for (int j : views::iota(1, n + 1))
      if (v(i, j))
        for (int k : views::iota(-4, 5))
          ck(i + k, j), ck(i, j + k);
}