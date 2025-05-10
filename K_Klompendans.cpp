// 这是一道分层图广度优先搜索题
// 判断所有可以到达的点 有两种交替的走姿
// 所以需要分层图 但答案需要取交集
#include <algorithm>
#include <array>
#include <iostream>
#include <print>
#include <queue>
#include <ranges>
#include <vector>

int main() {
  int n, a, b, c, d, ans{};
  std::cin >> n >> a >> b >> c >> d;
  auto v = std::vector<bool>(n * n * 2);
  const auto t = std::vector{std::vector<std::pair<int, int>>{{a, b},
                                                              {a, -b},
                                                              {-a, b},
                                                              {-a, -b},
                                                              {b, a},
                                                              {b, -a},
                                                              {-b, a},
                                                              {-b, -a}},
                             std::vector<std::pair<int, int>>{{c, d},
                                                              {c, -d},
                                                              {-c, d},
                                                              {-c, -d},
                                                              {d, c},
                                                              {d, -c},
                                                              {-d, c},
                                                              {-d, -c}}};
  auto getvid = [n](int x, int y, int op) { return op * n * n + y * n + x; };
  auto chk = [n](int x) { return std::clamp(x, 0, n - 1) == x; };
  std::queue<std::array<int, 3>> s;
  auto Push = [&](int x, int y, int op) {
    if (int vid = getvid(x, y, op); chk(x) && chk(y) && !v.at(vid))
      s.push({x, y, op}), v[vid] = 1;
  };
  Push(0, 0, 0), Push(0, 0, 1);
  while (!s.empty()) {
    auto [X, Y, op] = s.front();
    s.pop();
    for (const auto &p : t[op]) {
      auto [x, y] = p;
      Push(x+X, y+Y, op ^ 1);
    }
  }
  for (const auto &&o : std::views::zip(v | std::ranges::views::take(n * n),
                                        v | std::ranges::views::drop(n * n))) {
    auto &[x, y] = o;
    ans += x | y;
  }
  std::print("{}", ans);
}