// 智能指针性能不如裸指针，可能受锁和内存管理开销影响 大约5倍差异
#include <algorithm> // For std::swap, std::reverse
#include <array>
#include <iostream>
#include <memory>
#include <vector>

// Define The Modulo Constant
const int MOD = 51061;

class LinkCutTree {
private:
  struct Node : std::enable_shared_from_this<Node> {
    int id;
    long long value = 1; // Node's own value, modulo MOD
    long long subtree_sum =
        1;        // Sum of values in this node's Splay subtree, modulo MOD
    int size = 1; // Number of nodes in this Splay subtree

    bool reversed = false;
    long long lazy_add = 0; // Lazy addition tag, modulo MOD
    long long lazy_mul = 1; // Lazy multiplication tag, modulo MOD

    std::weak_ptr<Node> parent_weak;
    std::array<std::shared_ptr<Node>, 2> children;

    Node(int _id) : id(_id) {}

    [[nodiscard]] __attribute__((always_inline)) auto get_splay_parent() const
        -> std::shared_ptr<Node> {
      return parent_weak.lock();
    }
  };

  using NodePtr = std::shared_ptr<Node>;
  std::vector<NodePtr> nodes_map;
  std::vector<NodePtr> splay_path_cache_;

  [[nodiscard]] __attribute__((always_inline)) auto
  normalize(long long val) const {
    val %= MOD;
    if (val < 0)
      val += MOD;
    return val;
  }

  __attribute__((always_inline)) inline void push_up(const NodePtr &x) {
    if (!x)
      return;
    x->size = 1;
    x->subtree_sum = x->value; // x->value should be up-to-date

    const auto &ch0 = x->children[0];
    const auto &ch1 = x->children[1];

    if (ch0) {
      x->size += ch0->size;
      x->subtree_sum = normalize(x->subtree_sum + ch0->subtree_sum);
    }
    if (ch1) {
      x->size += ch1->size;
      x->subtree_sum = normalize(x->subtree_sum + ch1->subtree_sum);
    }
  }

  // Applies tags (mul_val, add_val) to 'node' itself: updates its value, sum,
  // and its own lazy tags.
  __attribute__((always_inline)) inline void
  apply_tags_to_node(const NodePtr &node, long long mul_val,
                     long long add_val) {
    if (!node)
      return;

    // Apply to node's actual value
    node->value = normalize(node->value * mul_val + add_val);
    // Apply to node's subtree sum (this node is root of splay tree, tag applies
    // to all nodes in it)
    node->subtree_sum =
        normalize(node->subtree_sum * mul_val + add_val * node->size);

    // Compose with node's existing lazy tags (these will be pushed to its
    // children later)
    node->lazy_mul = normalize(node->lazy_mul * mul_val);
    node->lazy_add = normalize(node->lazy_add * mul_val + add_val);
  }

  __attribute__((always_inline)) inline void push_down(const NodePtr &x) {
    if (!x)
      return;

    // Handle reversal first
    if (x->reversed) {
      x->reversed = false; // Clear parent's flag
      std::swap(x->children[0], x->children[1]);
      if (x->children[0])
        x->children[0]->reversed ^= true;
      if (x->children[1])
        x->children[1]->reversed ^= true;
    }

    // Handle multiply and add tags
    if (x->lazy_mul != 1 || x->lazy_add != 0) {
      long long current_lazy_mul = x->lazy_mul;
      long long current_lazy_add = x->lazy_add;

      // Apply parent's (x's) tags to children
      if (x->children[0]) {
        apply_tags_to_node(x->children[0], current_lazy_mul, current_lazy_add);
      }
      if (x->children[1]) {
        apply_tags_to_node(x->children[1], current_lazy_mul, current_lazy_add);
      }

      // Reset parent's (x's) tags
      x->lazy_mul = 1;
      x->lazy_add = 0;
    }
  }

  [[nodiscard]] __attribute__((always_inline)) auto
  is_splay_tree_root(const NodePtr &x) const {
    if (!x || x->parent_weak.expired())
      return true;
    auto p = x->parent_weak.lock();
    return !p || (p->children[0] != x && p->children[1] != x);
  }

  __attribute__((always_inline)) inline void
  set_splay_child(const NodePtr &p, const NodePtr &c, int dir) {
    if (p)
      p->children[dir] = c;
    if (c)
      c->parent_weak = p; // Set child's splay parent
  }

  [[nodiscard]] __attribute__((always_inline)) auto
  get_splay_child_direction_from_parent(const NodePtr &x,
                                        const NodePtr &y) const {
    return y->children[1] == x;
  }

  __attribute__((always_inline)) inline void rotate_node(const NodePtr &x) {
    NodePtr y = x->get_splay_parent();
    NodePtr z = y->get_splay_parent();

    int dir_x_vs_y = get_splay_child_direction_from_parent(x, y);

    set_splay_child(y, x->children[dir_x_vs_y ^ 1], dir_x_vs_y);

    x->parent_weak = z;
    if (z && !is_splay_tree_root(y)) {
      set_splay_child(z, x, get_splay_child_direction_from_parent(y, z));
    }

    set_splay_child(x, y, dir_x_vs_y ^ 1);

    push_up(y);
    // push_up(x) done by splay or caller
  }

  void splay(const NodePtr &x_node_to_splay) {
    if (!x_node_to_splay)
      return;

    splay_path_cache_.clear();
    NodePtr current_ancestor = x_node_to_splay;
    while (true) {
      splay_path_cache_.push_back(current_ancestor);
      if (is_splay_tree_root(current_ancestor))
        break;
      current_ancestor = current_ancestor->get_splay_parent();
      if (!current_ancestor) { /* Path break error guard */
        return;
      }
    }

    std::ranges::reverse(splay_path_cache_);
    for (const auto &node_on_path : splay_path_cache_) {
      if (node_on_path)
        push_down(node_on_path);
    }

    while (!is_splay_tree_root(x_node_to_splay)) {
      NodePtr y = x_node_to_splay->get_splay_parent();
      if (!y) { /* Should not happen */
        break;
      }

      if (!is_splay_tree_root(y)) {
        NodePtr z = y->get_splay_parent();
        if (!z) { /* Should not happen */
          rotate_node(x_node_to_splay);
          continue;
        }

        if (get_splay_child_direction_from_parent(x_node_to_splay, y) ==
            get_splay_child_direction_from_parent(y, z)) {
          rotate_node(y);
        } else {
          rotate_node(x_node_to_splay);
        }
      }
      rotate_node(x_node_to_splay);
    }
    push_up(x_node_to_splay);
  }

public:
  explicit LinkCutTree(int n_nodes_capacity)
      : nodes_map(static_cast<size_t>(n_nodes_capacity) + 1, nullptr) {
    if (n_nodes_capacity > 0) {
      splay_path_cache_.reserve(static_cast<size_t>(n_nodes_capacity) + 1);
    }
  }

  void initialize_nodes(int n_nodes_count) {
    if (static_cast<size_t>(n_nodes_count + 1) > nodes_map.size()) {
      nodes_map.resize(static_cast<size_t>(n_nodes_count) + 1, nullptr);
    }
    for (int i = 1; i <= n_nodes_count; ++i) {
      nodes_map[i] = std::make_shared<Node>(i);
    }
  }

  [[nodiscard]] __attribute__((always_inline)) auto get_node_ptr(int id)
      -> NodePtr {
    if (id >= 1 && static_cast<size_t>(id) < nodes_map.size()) {
      return nodes_map[id];
    }
    return nullptr;
  }

  void access(int x_id) {
    NodePtr x_target_node = get_node_ptr(x_id);
    if (!x_target_node)
      return;

    NodePtr last_child_on_new_path = nullptr;
    NodePtr current_node_climbing_up = x_target_node;

    while (current_node_climbing_up) {
      splay(current_node_climbing_up);
      // Detach old right child (preferred path segment below
      // current_node_climbing_up)
      if (current_node_climbing_up->children[1]) {
        current_node_climbing_up->children[1]
            ->parent_weak.reset(); // Becomes a splay root
      }
      // Attach new right child (segment of path from x_target_node upwards)
      set_splay_child(current_node_climbing_up, last_child_on_new_path, 1);
      push_up(current_node_climbing_up);

      last_child_on_new_path = current_node_climbing_up;
      current_node_climbing_up =
          current_node_climbing_up->get_splay_parent(); // Move to LCT parent
    }
    splay(x_target_node); // Splay target node to root of its aux tree for
                          // consistent query/update point
  }

  void make_root(int x_id) {
    NodePtr x = get_node_ptr(x_id);
    if (!x)
      return;
    access(x_id); // x is splayed to root of preferred path. Ancestors are on
                  // its "left" in splay tree.
    x->reversed ^= true;
    push_down(x); // Propagate reversal to children immediately
  }

  [[nodiscard]] __attribute__((always_inline)) auto find_root_id(int x_id)
      -> int {
    NodePtr x = get_node_ptr(x_id);
    if (!x)
      return 0;
    access(x_id);

    NodePtr curr = x;
    while (curr) {
      push_down(curr); // Ensure state is clean before traversing down
      if (!curr->children[0])
        break;
      curr = curr->children[0];
    }
    if (curr) {
      splay(curr); // Splay actual root to amortize and update
      return curr->id;
    }
    return 0;
  }

  // Makes path u-v active, with v as the splay root of the aux tree for this
  // path.
  void split(int u_id, int v_id) {
    make_root(u_id);
    access(v_id);
  }

  void link(int x_id, int y_id) {
    NodePtr x_node = get_node_ptr(x_id);
    NodePtr y_node = get_node_ptr(y_id);
    if (!x_node || !y_node || x_id == y_id)
      return;

    make_root(x_node->id);
    // If y_node is already in x_node's component, find_root(y_id) will return
    // x_id
    if (find_root_id(y_node->id) != x_node->id) {
      x_node->parent_weak = y_node; // x_node's LCT parent becomes y_node
    }
  }

  void cut(int x_id, int y_id) {
    NodePtr x_node = get_node_ptr(x_id);
    NodePtr y_node = get_node_ptr(y_id);
    if (!x_node || !y_node)
      return;

    split(x_id, y_id); // Exposes path x-y. y_node is splay root. x_node is LCT
                       // parent of y_node. In y_node's splay tree, x_node must
                       // be its left child.

    // Check conditions: y_node splay_root, x_node is its left child,
    // x_node has no right splay child (meaning y is immediate LCT child of x in
    // path), and x_node's splay parent is indeed y_node.
    if (y_node->children[0] == x_node && x_node->children[1] == nullptr &&
        x_node->get_splay_parent() == y_node) {
      y_node->children[0] = nullptr; // Sever Splay link from y to x
      x_node->parent_weak.reset();   // Sever Splay parent link from x to y
      push_up(y_node);               // y_node's aggregates (size, sum) change
    }
  }

  void path_add(int u_id, int v_id, int c_val_raw) {
    NodePtr u_node = get_node_ptr(u_id);
    NodePtr v_node = get_node_ptr(v_id);
    if (!u_node || !v_node)
      return;
    long long c_val = normalize(c_val_raw);

    split(u_id, v_id);
    NodePtr path_repr_node =
        get_node_ptr(v_id); // This is the splay root for path u-v
    if (path_repr_node) {
      apply_tags_to_node(path_repr_node, 1, c_val); // Add c_val, mul by 1
    }
  }

  void path_multiply(int u_id, int v_id, int c_val_raw) {
    NodePtr u_node = get_node_ptr(u_id);
    NodePtr v_node = get_node_ptr(v_id);
    if (!u_node || !v_node)
      return;
    long long c_val = normalize(c_val_raw);

    split(u_id, v_id);
    NodePtr path_repr_node = get_node_ptr(v_id);
    if (path_repr_node) {
      apply_tags_to_node(path_repr_node, c_val, 0); // Mul by c_val, add 0
    }
  }

  [[nodiscard]] __attribute__((always_inline)) auto query_path_sum(int u_id,
                                                                   int v_id)
      -> long long {
    NodePtr u_node = get_node_ptr(u_id);
    NodePtr v_node = get_node_ptr(v_id);
    if (!u_node || !v_node)
      return 0;

    // Optional: Check for connectivity beforehand for strictness, though
    // split/access handles it structurally. if (find_root_id(u_id) !=
    // find_root_id(v_id)) return 0; // If query on disconnected nodes is 0

    split(u_id, v_id);
    NodePtr path_repr_node = get_node_ptr(v_id);
    // After split(u,v), path_repr_node (which is v_node) has its subtree_sum
    // correctly representing path u-v
    return path_repr_node ? path_repr_node->subtree_sum : 0;
  }
};

auto main() -> int {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  int n_nodes, q_ops;
  std::cin >> n_nodes >> q_ops;

  LinkCutTree lct(n_nodes);
  if (n_nodes > 0) {
    lct.initialize_nodes(n_nodes);
  }

  for (int i = 0; i < n_nodes - 1; ++i) {
    int u, v;
    std::cin >> u >> v;
    if (u >= 1 && u <= n_nodes && v >= 1 && v <= n_nodes) {
      lct.link(u, v);
    }
  }

  for (int k = 0; k < q_ops; ++k) {
    char op_char;
    std::cin >> op_char;

    if (op_char == '+') {
      int u, v, c;
      std::cin >> u >> v >> c;
      if (u >= 1 && u <= n_nodes && v >= 1 && v <= n_nodes)
        lct.path_add(u, v, c);
    } else if (op_char == '-') {
      int u1, v1, u2, v2;
      std::cin >> u1 >> v1 >> u2 >> v2;
      if (u1 >= 1 && u1 <= n_nodes && v1 >= 1 && v1 <= n_nodes && u2 >= 1 &&
          u2 <= n_nodes && v2 >= 1 && v2 <= n_nodes) {
        lct.cut(u1, v1);
        lct.link(u2, v2);
      }
    } else if (op_char == '*') {
      int u, v, c;
      std::cin >> u >> v >> c;
      if (u >= 1 && u <= n_nodes && v >= 1 && v <= n_nodes)
        lct.path_multiply(u, v, c);
    } else if (op_char == '/') {
      int u, v;
      std::cin >> u >> v;
      if (u >= 1 && u <= n_nodes && v >= 1 && v <= n_nodes) {
        long long result = lct.query_path_sum(u, v);
        std::cout << result << "\n";
      } else {
        std::cout << 0 << "\n";
      }
    }
  }
  return 0;
}