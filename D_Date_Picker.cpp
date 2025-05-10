#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

using namespace std;
using namespace views;
using u16 = uint16_t;

auto main() -> int {
  int d, h, ans{};
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
  for_each(execution::par_unseq, S.begin(), S.end(), [&ans, h, H, a](const auto s) {
    auto c = vector(H, u16{});
    for (auto &&[i, a] : enumerate(a) | filter([s](auto const &x) {
                           return (s >> get<0>(x)) & 1;
                         }))
      for (auto &&[x, y] : zip(c, a))
        x += y;
    ranges::sort(c, greater{});
    ans = max(ans, accumulate(c.begin(), c.begin() + h, 0));
  });
  print("{:.15f}", static_cast<double>(ans) / (d * h));
}
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