// // 贪心算法 注意边界情况
// #include <algorithm>
// #include <array>
// #include <iostream>
// #include <print>
// #include <ranges>
// #include <vector>

// using namespace std;
// using namespace views;

// auto main() -> int {
//   ios::sync_with_stdio(false), cin.tie(nullptr);
//   int n, k;
//   cin >> n >> k;
//   if (n == 1 || k == 1)
//     print("1"), exit(0);
//   if (n == 2)
//     print("2"), exit(0);
//   auto a = views::istream<int>(cin) | ranges::to<vector>();
//   ranges::sort(a);
//   int ans = 2, cur = 0;
//   k -= 2;
//   auto c = array{0, 0, 0};
//   for (int const &i : a | take(n - 2)) {
//     cur += i / 3, c[i % 3]++;
//     if (cur + c[2] + max(0, (c[1] - c[2] + 1) / 2) <= k)
//       ans++;
//     else
//       break;
//   }
//   print("{}", ans);
// }
