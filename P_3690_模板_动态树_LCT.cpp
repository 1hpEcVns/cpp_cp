// 指针性能不如直接数组索引
#include <cstdio>
#include <vector>

using namespace std;
char buf[100005], *p1 = buf, *p2 = buf;
char obuf[100005], *p3 = obuf, *p4 = obuf + 35;
auto gtchar() -> char {
  if (p1 == p2) [[unlikely]] {
    p1 = buf;
    p2 = p1 + fread(buf, 1, sizeof(buf), stdin);
    if (p1 == p2) {
      return EOF;
    }
  }
  return *p1++;
}
auto read() -> int {
  int ret = 0;
  char ch = gtchar();
  while (ch < '0' || ch > '9') [[unlikely]]
    ch = gtchar();
  while (ch >= '0' && ch <= '9') [[likely]] {
    ret = (ret << 3) + (ret << 1) + (ch ^ 48);
    ch = gtchar();
  }
  return ret;
}
void ptchar(char ch) {
  if (p3 == p4) [[unlikely]] {
    fwrite(obuf, 1, p4 - obuf, stdout);
    p3 = obuf;
  }
  *p3++ = ch;
}
void write(int x) {
  if (x > 9) [[likely]] {
    write(x / 10);
  }
  ptchar((x % 10) ^ 48);
}
struct FastIOFlusher {
  ~FastIOFlusher() {
    if (p3 != obuf) {
      fwrite(obuf, 1, p3 - obuf, stdout);
      p3 = obuf;
    }
  }
};
struct Node {
  Node *fa, *ch[2];
  int val, xr, tg;
  [[gnu::always_inline]] auto ptr() { return this; }
  [[gnu::always_inline]] auto pushup() {
    xr = val;
    if (ch[0])
      xr ^= ch[0]->xr;
    if (ch[1])
      xr ^= ch[1]->xr;
  }
  [[gnu::always_inline]] auto pushdown() {
    if (tg) {
      if (ch[0])
        ch[0]->tg ^= 1;
      if (ch[1])
        ch[1]->tg ^= 1;
      tg = 0;
      swap(ch[0], ch[1]);
    }
  }
  [[gnu::always_inline]] auto addson(Node *x, int d) {
    if (ch[d] = x, x)
      x->fa = this;
  }
  [[gnu::always_inline]] auto childtype() {
    return fa ? fa->ch[0] != this : true;
  }
  [[gnu::always_inline]] auto notroot() {
    return fa && (fa->ch[0] == this || fa->ch[1] == this);
  }
  auto down() -> void {
    if (notroot())
      fa->down();
    pushdown();
  }
  [[gnu::always_inline]] auto rotate() {
    Node *p = fa;
    auto d = childtype();
    if (p->notroot())
      p->fa->ch[p->childtype()] = this;
    fa = fa->fa;
    p->addson(ch[!d], d), addson(p, !d);
    p->pushup();
  }
  [[gnu::always_inline]] auto splay() {
    for (down(); notroot(); rotate())
      if (fa->notroot())
        childtype() ^ fa->childtype() ? rotate() : fa->rotate();
    pushup();
  }
  [[gnu::always_inline]] auto access(Node *i) {
    for (Node *j = nullptr; i; j = i, i = i->fa)
      i->splay(), i->addson(j, 1), i->pushup();
  }
  [[gnu::always_inline]] auto splay2root() { access(this), splay(); }
  [[gnu::always_inline]] auto mkroot() { splay2root(), tg ^= 1; }
  [[gnu::always_inline]] auto expose(Node *y) { mkroot(), y->splay2root(); }
  [[gnu::always_inline]] auto findfa(Node *i) {
    for (i->splay2root(); i->ch[0]; i = i->ch[0])
      ;
    return i;
  }
  [[gnu::always_inline]] auto connected(Node *i, Node *j) {
    return findfa(i) == findfa(j);
  }
  [[gnu::always_inline]] auto adjcent(Node *j) {
    return expose(j), j->ch[0] == this && !j->ch[1];
  }
  [[gnu::always_inline]] auto link(Node *j) {
    if (!connected(this, j))
      mkroot(), fa = j;
  }
  [[gnu::always_inline]] auto cut(Node *j) {
    if (adjcent(j))
      fa = j->ch[0] = nullptr;
  }
};

auto main() -> int {
  int n = read(), m = read();
  vector<Node> v(n);
  for (Node &i : v)
    i.xr = i.val = read();
  for (; m--;) {
    int o = read(), x = read(), y = read();
    auto i = v[x - 1].ptr(), j = v[y - 1].ptr();
    if (o == 0)
      i->expose(j), write(j->xr), ptchar('\n');
    if (o == 1)
      i->link(j);
    if (o == 2)
      i->cut(j);
    if (o == 3)
      i->splay(), i->val = y;
  }
  fwrite(obuf, 1, p3 - obuf, stdout);
}