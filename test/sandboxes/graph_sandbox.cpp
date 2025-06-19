/**
 * \file test/sandboxes/graph_sandbox.cpp
 **/
#include <print>
#include <queue>
#include <stack>

#include "tensorlib.hpp"

namespace tensor {
  namespace graph {

    struct bool_wrapper {
      bool_wrapper(bool v = false) : value(v) {}
      bool value = false;
      operator bool() { return value; }
      constexpr auto operator<=>(const bool_wrapper&) const = default;
    };

    struct weighted_node {
      natural_t id;
      real_t weight;

      constexpr auto operator<=>(const weighted_node&) const = default;
    };

    template <typename Fn>
    void bfs(graph& g, natural_t start_node, Fn visit_node);

    template <typename Fn>
    void dfs(graph& g, natural_t start_node, Fn visit_node);

  }  // namespace graph
}  // namespace tensor

namespace tg = tensor::graph;

int main() {
  tg::graph g = tg::new_graph(
    {
      { 1, 2 },
      { 0 },
      { 0, 3, 4 },
      { 2 },
      { 2 },
    }
  );
  tensor::dyn_matrix adj_matrix = tg::get_adjacency_matrix(g);
  std::println("Graph: {}", tensor::as_string(g));
  std::println("Adjacency Matrix:\n{}", tensor::as_string(adj_matrix));

  tg::bfs(g, 0, [](const tg::graph& g, tensor::natural_t node_id, tensor::real_t weight) {
    std::println("Visited node {} with weight {}", node_id, weight);
  });
  std::println("");
  tg::dfs(g, 0, [](const tg::graph& g, tensor::natural_t node_id, tensor::real_t weight) {
    std::println("Visited node {} with weight {}", node_id, weight);
  });

  return 0;
}

namespace tensor {
  namespace graph {

    template <typename Fn>
    void bfs(graph& g, natural_t start_node, Fn visit_node) {
      std::vector<bool_wrapper> visited(g.nodes.size(), false);
      std::queue<weighted_node> queue;

      visited[start_node] = true;

      real_t edge_weight = 0.f;
      if (detail::epsilon_gt(get_edge_weight(g, start_node, start_node), 0.f)) {
        edge_weight = get_edge_weight(g, start_node, start_node);
      }
      queue.push({ start_node, edge_weight });

      while (!queue.empty()) {
        weighted_node node = queue.front();
        queue.pop();

        visit_node(g, node.id, node.weight);

        for (const auto& edge : g.edges[node.id]) {
          if (!visited[edge.to]) {
            visited[edge.to] = true;
            queue.push({ edge.to, edge.weight });
          }
        }
      }
    }

    template <typename Fn>
    void dfs(graph& g, natural_t start_node, Fn visit_node) {
      std::vector<bool_wrapper> visited(g.nodes.size(), false);
      std::stack<weighted_node> stack;

      visited[start_node] = true;

      real_t edge_weight = 0.f;
      if (g.has_loops && edge_exists(g, start_node, start_node)) {
        edge_weight = get_edge_weight(g, start_node, start_node);
      }
      stack.push({ start_node, edge_weight });

      while (!stack.empty()) {
        weighted_node node = stack.top();
        stack.pop();

        visit_node(g, node.id, node.weight);

        for (const auto& edge : g.edges[node.id]) {
          if (!visited[edge.to]) {
            visited[edge.to] = true;
            stack.push({ edge.to, edge.weight });
          }
        }
      }
    }

  }  // namespace graph
}  // namespace tensor