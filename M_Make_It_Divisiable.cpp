#include <algorithm>
#include <cstddef>
#include <iostream>
#include <print>
#include <random>
#include <ranges>
#include <vector>

auto main() -> int {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  auto T = 0;
  std::cin >> T;
  while (T--) {
    auto n = 0;
    std::cin >> n;
    auto k = 0;
    std::cin >> k;
    std::println("{} {}", n, k);
    auto a = std::vector<int>(n);
    auto t = std::vector(n, std::pair(-1, -1));
    auto st = std::vector<int>();
    for (auto i : std::views::iota(0, n)) {
      std::cin >> a[i];
      auto l = -1;
      while (!st.empty() && a[i] < a[st.back()]) {
        if (l != -1) {
          t[st.back()].second = l;
        }
        l = st.back();
        st.pop_back();
      }
      if (l != -1) {
        t[i].first = l;
      }
      st.push_back(i);
    }
    for (size_t sz = st.size(), i = sz - 1, j = sz - 2; i; --i, --j) {
      t[st[j]].second = st[i];
    }
    auto v = std::vector<std::pair<int, int>>();
    for (auto const &[i, t] : std::views::zip(a, t)) {
      auto const &[l, r] = t;
      if (l != -1 && i != a[l]) {
        v.emplace_back(i, a[l] - i);
      }
      if (r != -1 && i != a[r]) {
        v.emplace_back(i, a[r] - i);
      }
    }
    if (v.empty()) {
      std::println("{} {}", k, 1LL * k * (k + 1) / 2);
      continue;
    }
    std::ranges::sort(v);
    v.resize(std::ranges::unique(v).begin() - v.begin());
    std::ranges::shuffle(v, std::mt19937(std::random_device()()));
    auto chk = [v, k](int x) {
      if (k < x || x < 1) {
        return false;
      }
      for (auto const &[i, t] : v) {
        if (t % (i + x)) {
          return false;
        }
      }
      return true;
    };
    auto cnt = 0;
    auto sum = 0LL;
    auto const [i, j] = v.front();
    for (auto p1 = 1; p1 * p1 <= j; ++p1) {
      if (j % p1 == 0) {
        auto p2 = j / p1;
        p1 -= i;
        if (chk(p1)) {
          ++cnt;
          sum += p1;
        }
        if (p1 != p2) {
          p2 -= i;
          if (chk(p2)) {
            ++cnt;
            sum += p2;
          }
        }
      }
    }
    std::println("{} {}", cnt, sum);
  }
}