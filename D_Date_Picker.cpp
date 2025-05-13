// gcc错误理解了reduce_op的含义并以简单加法的方法处理规约而不是给定的初值0
// 本代码clang编译器编译的结果是正确的 gcc编译器可以使用下面注释掉的代码
#include <algorithm>
#include <cstdint>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

using namespace std;
using namespace views;
using u16 = uint16_t;

auto main() -> int {
  int d, h;
  auto constexpr D = 7, H = 24;
  auto const &a = views::iota(0, D) |
                  views::transform([&]([[maybe_unused]] int const i) {
                    return views::iota(0, H) |
                           views::transform([&]([[maybe_unused]] int const j) {
                             char c;
                             cin >> c;
                             return c == '.';
                           }) |
                           ranges::to<vector>();
                  }) |
                  ranges::to<vector>();
  cin >> d >> h;
  const auto S = views::iota(static_cast<u16>(1), static_cast<u16>(1 << D)) |
                 filter([d](u16 const &s) { return popcount(s) == d; }) |
                 ranges::to<vector>();
  print("{:.15f}",
        static_cast<double>(reduce(
            execution::par_unseq, S.begin(), S.end(), 0,
            [h, H, a](int const ans, int const s) {
              auto c = vector(H, u16{});
              for (auto &&[i, a] : enumerate(a) | filter([s](auto const &x) {
                                     return (s >> get<0>(x)) & 1;
                                   }))
                for (auto &&[x, y] : zip(c, a))
                  x += y;
              ranges::sort(c, greater{});
              return max(ans, reduce(c.begin(), c.begin() + h, 0));
            })) /
            (d * h));
}

// 原代码使用了并行计算 reduce 配合 execution::par_unseq 来加速
// 这是一个暴力解法的加速方案 是一个有益的尝试
// 说明了range for可以轻松使用并行计算来加速
// 注释掉的是常规for循环
// for (u16 s : views::iota(static_cast<u16>(1), static_cast<u16>(1 << D)) |
//                    filter([d](u16 const &s) { return popcount(s) == d; })) {
//     auto c = vector(H, u16{});
//     for (auto &&[i, a] : enumerate(a) | filter([s](auto const &x) {
//                            return (s >> get<0>(x)) & 1;
//                          }))
//       for (auto &&[x, y] : zip(c, a))
//         x += y;
//     ranges::sort(c, greater{});
//     ans = max(ans, accumulate(c.begin(), c.begin() + h, 0));
// }