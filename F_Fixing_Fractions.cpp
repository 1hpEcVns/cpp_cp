#include <cstdint>
#include <iostream>
#include <print>
#include <ranges>
#include <string>

using namespace std;
using namespace views;

auto main() -> int {
  int64_t a, b, c, d;
  cin >> a >> b >> c >> d;
  auto A = to_string(a), B = to_string(b);
  for (int s : views::iota(0, 1 << A.size())) {
    string X, Y, Ta, Tb;
    for (auto &&[i, c] : enumerate(A))
      (s >> i & 1 ? X : Ta) += c;
    if (X.empty() || X[0] == '0')
      continue;
    int64_t x = stoll(X);
    __int128 y = static_cast<__int128>(x) * d / c;
    if (y * c != x * d || y > b)
      continue;
    Y = to_string(static_cast<int64_t>(y));
    auto p = Y.begin();
    for (char &c : B)
      if (p != Y.end() && c == *p)
        ++p;
      else
        Tb += c;
    if (p != Y.end())
      continue;
    bool f = true;
    array<int, 10> c{};
    for (char &i : Ta)
      ++c[i - '0'];
    for (char &i : Tb)
      --c[i - '0'];
    for (int &i : c)
      if (i)
        f = false;
    if (f)
      print("possible\n{} {}", X, Y), exit(0);
  }
  print("impossible");
}