// gcc错误理解了reduce_op的含义并以简单加法的方法处理规约而不是给定的初值0
// 本代码clang编译器编译的结果是正确的 gcc编译器可以使用下面注释掉的代码
// GCC的 `std::reduce` 并行实现在某些情况下未能正确地将幺元（identity
// element，即`0`） 应用于其并行归约树的叶子节点或初始计算单元。
// 它错误地用输入序列中的原始元素替代了本应由幺元或从幺元派生的累积值占据的位置。
// 这导致了这些初始计算单元产生的结果从语义上就偏离了正确的归约路径。
// 因此，尽管所使用的二元操作符（`std::max`）本身满足结合律，
// 但由于起始操作数被错误地初始化，
// 整个依赖于结合律的并行归约过程最终得出了错误的累积结果。

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