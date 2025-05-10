#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

#define ts views::transform
#define ia views::iota
#define vc vector
#define to ranges::to
#define en enumerate
#define is views::istream
#define rs ranges::sort

using namespace std;
using namespace views;

auto main() -> int {
  ios::sync_with_stdio(false), cin.tie(nullptr);
  int n;
  auto a = is<int>(cin) | to<vc>();
  rs(a);
  for (auto &&[i, x] : en(a))
    cout << x << " \n"[i == n - 1];
} 