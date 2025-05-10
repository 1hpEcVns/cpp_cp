// 这是一道构造题
// 输出比较复杂 需要考虑很多情况
#include <array>
#include <iostream>
#include <print>

using namespace std;

constexpr int N = 1000001;

array<int, N> a;

auto main() -> int {
  ios::sync_with_stdio(false), cin.tie(nullptr);
  int n, c;
  bool f = true;
  string last;
  cin >> n;
  if (n == 1)
    cin >> c, print("{}", c), exit(0);
  for (; n--; a[c]++)
    cin >> c;
  auto out = [&](string const &x) -> void {
    if (f)
      print("{}", x);
    else
      print("*{}", x);
    f = false;
  };
  auto Out = [&](int const i) -> void {
    while (a[i]--)
      out(format("{}", i));
  };
  auto Os = [&](string const &x) -> void {
    if (last.empty())
      last = x;
    else
      out(last), last = x;
  };
  if (a[1] == 1) {
    for (int i = 2; i < N; ++i)
      if (a[i]) {
        if (f)
          out(format("(1+{})", i)), --a[i];
        Out(i);
      }
    exit(0);
  }
  while (a[1] && a[2])
    Os("(1+2)"), --a[1], --a[2];
  while (a[1] > 2)
    Os("(1+1+1)"), a[1] -= 3;
  if (a[1] == 2)
    Os("(1+1)");
  if (a[1] == 1)
    last.insert(1, "1+");
  if (!last.empty())
    out(last);
  for (int i = 2; i < N; ++i)
    Out(i);
}