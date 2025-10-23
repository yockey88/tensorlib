/**
 * \file graph/graph.cpp
 **/
#include "graph/graph.hpp"

#include <sstream>
#include <stack>

#include "core/types.hpp"

namespace tensor {
  namespace graph {

    std::string graph::write_string(const graph& g) {
      std::ostringstream oss;
      oss << "Graph with " << g.nodes.size() << " nodes and " << count_edges(g) << " edges.\n";
      for (const auto& node : g.nodes) {
        oss << "Node ID: " << node.id << "\n";
      }
      for (size_t i = 0; i < g.edges.size(); ++i) {
        oss << "Edges from node " << i << ": ";
        for (const auto& edge : g.edges[i]) {
          oss << "(" << edge.from << " -> " << edge.to << ", weight: " << edge.weight << ") ";
        }
        if (i != g.edges.size() - 1) {
          oss << "\n";
        }
      }
      return oss.str();
    }

    graph empty_graph(natural_t n) {
      graph g;
      g.nodes.resize(n);
      g.edges.resize(n);
      for (natural_t i = 0; i < n; ++i) {
        g.nodes[i].id = i;
      }
      return g;
    }

    graph new_graph(const std::vector<std::vector<natural_t>>& adj_list) {
      graph g;
      g.nodes.resize(adj_list.size());
      g.edges.reserve(adj_list.size());

      for (size_t i = 0; i < adj_list.size(); ++i) {
        g.nodes[i].id = i;
        auto& edges = g.edges.emplace_back();
        for (const auto& edge : adj_list[i]) {
          edges.emplace_back(graph::edge{ i, edge, 1.f });
          g.num_edges++;
        }
      }

      return g;
    }

    graph get_Kn(natural_t n, bool with_loops) {
      graph g;
      g.nodes.resize(n);
      g.edges.reserve(n);

      for (natural_t i = 0; i < n; ++i) {
        g.nodes[i].id = i;
        auto& edges = g.edges.emplace_back();

        for (natural_t j = 0; j < n; ++j) {
          if (i == j && !with_loops) {
            continue;
          }
          edges.emplace_back(graph::edge{ i, j, 1.0 });  // weight is 1.0 for all edges
          g.num_edges++;
        }
      }

      return g;
    }

    dyn_matrix get_adjacency_matrix(const graph& g) {
      natural_t n = g.nodes.size();
      dyn_matrix adj_matrix(n, n);

      for (size_t i = 0; i < g.edges.size(); ++i) {
        for (const auto& edge : g.edges[i]) {
          adj_matrix(i, edge.to) = edge.weight;
        }
      }

      return adj_matrix;
    }

    natural_t get_node_outdegree(const graph& g, natural_t node_id) {
      if (node_id >= g.edges.size()) {
        return 0;
      }
      return g.edges[node_id].size();
    }

    natural_t get_node_indegree(const graph& g, natural_t node_id) {
      size_t indegree = 0;
      for (size_t i = 0; i < g.edges.size(); ++i) {
        if (i == node_id && !g.has_loops) {
          continue;
        }

        for (const auto& edge : g.edges[i]) {
          if (edge.to == node_id) {
            indegree++;
          }
        }
      }

      return indegree;
    }

    natural_t get_node_degree(const graph& g, natural_t node_id) {
      return get_node_outdegree(g, node_id) + get_node_indegree(g, node_id);
    }

    natural_t count_edges(const graph& g) {
      natural_t count = 0;
      for (const auto& edges : g.edges) {
        count += edges.size();
      }
      return count;
    }

    void add_edge(graph& g, natural_t from, natural_t to, real_t weight, bool directed, bool force) {
      add_edge(g, graph::edge{ from, to, weight }, directed, force);
    }

    void add_edge(graph& g, const graph::edge& e, bool directed, bool force) {
      if (e.from == e.to && !g.has_loops && !force) {
        /// log error ?
        return;
      }

      auto edge_itr = std::ranges::find(g.edges[e.from], e);
      if (edge_itr != g.edges[e.from].end()) {
        edge_itr->weight = e.weight;
      } else {
        g.edges[e.from].emplace_back(e);
        if (e.from == e.to) {
          g.has_loops = true;
        }
      }
    }

    bool remove_edge(graph& g, natural_t from, natural_t to) {
      return remove_edge(g, graph::edge{ from, to, 0 });
    }

    bool remove_edge(graph& g, const graph::edge& e) {
      auto edge_itr = std::ranges::find(g.edges[e.from], e);
      if (edge_itr != g.edges[e.from].end()) {
        g.edges[e.from].erase(edge_itr);
        g.num_edges--;
        return true;
      } else {
        return false;
      }
    }

    void set_edge_weight(graph& g, natural_t from, natural_t to, real_t weight, bool undirected, bool force) {
      set_edge_weight(g, graph::edge{ from, to, weight }, undirected, force);
    }

    void set_edge_weight(graph& g, const graph::edge& e, bool undirected, bool force) {
      if (e.from == e.to && !g.has_loops && !force) {
        /// log error ?
        return;
      }

      add_edge(g, e, true, force);
    }

    real_t get_edge_weight(const graph& g, natural_t from, natural_t to) {
      return get_edge_weight(g, graph::edge{ from, to, 0 });
    }

    real_t get_edge_weight(const graph& g, const graph::edge& e) {
      auto edge_itr = std::ranges::find(g.edges[e.from], e);
      if (edge_itr != g.edges[e.from].end()) {
        return edge_itr->weight;
      }
      return 0.f;
    }

    bool has_cycle(const graph& g) {
      return topological_sort(g).empty();
    }

    bool edge_exists(const graph& g, natural_t from, natural_t to) {
      return edge_exists(g, graph::edge{ from, to, 0 });
    }

    bool edge_exists(const graph& g, const graph::edge& e) {
      return std::ranges::find(g.edges[e.from], e) != g.edges[e.from].end();
    }

    std::vector<graph::edge>::iterator get_edge(graph& g, natural_t from, natural_t to) {
      auto edge_itr = std::ranges::find(g.edges[from], graph::edge{ from, to, 0 });
      if (edge_itr != g.edges[from].end()) {
        return edge_itr;
      }
      return g.edges[from].end();
    }

    void set_loop_weights(graph& g, real_t weight) {
      for (size_t i = 0; i < g.nodes.size(); ++i) {
        set_edge_weight(g, i, i, weight, false, true);
      }

      if (!g.has_loops) {
        g.has_loops = true;
      }
    }

    namespace detail {
      namespace {

        struct bool_wrapper {
          bool_wrapper(bool v = false) : value(v) {}
          bool value = false;
          operator bool() { return value; }
          constexpr auto operator<=>(const bool_wrapper&) const = default;
        };

        static inline void topo_sort_dfs_visit_node(const graph& g, natural_t node, std::vector<bool_wrapper>& temp_marked, std::vector<bool_wrapper>& perm_marked, std::vector<natural_t>& sorted_nodes) {
          temp_marked[node] = true;

          for (const auto& edge : g.edges[node]) {
            if (!temp_marked[edge.to]) {
              topo_sort_dfs_visit_node(g, edge.to, temp_marked, perm_marked, sorted_nodes);
            }
          }

          perm_marked[node] = true;
          sorted_nodes.push_back(node);
        }

      }  // namespace
    }  // namespace detail

    std::vector<natural_t> topological_sort(const graph& g) {
      if (g.has_loops) {
        return std::vector<natural_t>{};
      }

      std::vector<natural_t> sorted_nodes;

      //// see if there are nodes with no incoming edges to see if we can use Kahn
      std::stack<natural_t> no_edge_nodes;
      for (natural_t i = 0; i < g.nodes.size(); ++i) {
        if (get_node_indegree(g, i) == 0) {
          no_edge_nodes.push(i);
        }
      }

      natural_t clear_edge_lists = 0;
      if (no_edge_nodes.empty()) {
        /// if no nodes with indegree = 0, use DFS Algorithm
        std::vector<detail::bool_wrapper> temp_marked(g.nodes.size(), false);
        std::vector<detail::bool_wrapper> perm_marked(g.nodes.size(), false);

        while (!std::ranges::all_of(perm_marked, [](const detail::bool_wrapper& b) { return b; })) {
          integer_t node = -1;
          for (natural_t i = 0; i < g.nodes.size(); ++i) {
            if (!perm_marked[i]) {
              node = i;
              break;
            }
          }
          TENSORLIB_ASSERT(node >= 0, "No nodes to process");
          if (temp_marked[node]) {
            /// if we have a cycle, return empty
            return std::vector<natural_t>{};
          }

          detail::topo_sort_dfs_visit_node(g, node, temp_marked, perm_marked, sorted_nodes);
        }
      }
      /// if at least one node with indegree = 0, use Kahn's algorithm
      else {
        graph copy = g;

        while (!no_edge_nodes.empty()) {
          natural_t node = no_edge_nodes.top();
          no_edge_nodes.pop();

          sorted_nodes.push_back(node);

          for (natural_t i = 0; i < copy.nodes.size(); ++i) {
            if (copy.edges[i].empty()) {
              continue;
            }

            if (remove_edge(copy, node, i)) {
              if (get_node_indegree(g, i) == 0) {
                clear_edge_lists++;
                no_edge_nodes.push(i);
              }
            }
          }
        }
      }

      if (sorted_nodes.size() != g.nodes.size()) {
        /// disconnected ?
        return std::vector<natural_t>{};
      }

      bool has_cycle = clear_edge_lists != g.nodes.size();
      return has_cycle ? sorted_nodes : std::vector<natural_t>{};
    }

  }  // namespace graph
}  // namespace tensor