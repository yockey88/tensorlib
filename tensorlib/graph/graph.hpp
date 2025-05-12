/**
 * \file graph/graph.hpp
 **/
#ifndef TENSORLIB_GRAPH_GRAPH_HPP
#define TENSORLIB_GRAPH_GRAPH_HPP

#include <string>
#include <vector>

#include "core/types.hpp"

#include "linalgebra/dyn_matrix.hpp"

namespace tensor {
  namespace graph {

    struct graph {
      struct node {
        natural_t id;

        constexpr auto operator<=>(const node&) const = default;
      };

      struct edge {
        natural_t from;
        natural_t to;

        real_t weight;

        edge() = default;
        edge(natural_t f, natural_t t, real_t w)
            : from(f), to(t), weight(w) {}

        constexpr auto operator==(const edge& other) const {
          /// weight doesnt mean anything to equality
          return other.from == from && other.to == to;
        }
      };

      bool has_loops = false;

      natural_t num_edges = 0;
      std::vector<node> nodes;
      std::vector<std::vector<edge>> edges;

      static std::string write_string(const graph& g);
    };

    graph empty_graph(natural_t n = 0);

    graph new_graph(const dyn_matrix& adj_matrix);
    graph new_graph(const std::vector<std::vector<natural_t>>& adj_list);

    graph get_Kn(natural_t n, bool with_loops = false);

    dyn_matrix get_adjacency_matrix(const graph& g);

    natural_t get_node_outdegree(const graph& g, natural_t node_id);
    natural_t get_node_indegree(const graph& g, natural_t node_id);
    natural_t get_node_degree(const graph& g, natural_t node_id);

    natural_t count_edges(const graph& g);

    void add_edge(graph& g, natural_t from, natural_t to, real_t weight = 1.f, bool directed = true, bool force = false);
    void add_edge(graph& g, const graph::edge& e, bool directed = true, bool force = false);

    bool remove_edge(graph& g, natural_t from, natural_t to);
    bool remove_edge(graph& g, const graph::edge& e);

    void set_edge_weight(graph& b, natural_t from, natural_t to, real_t weight, bool directed = true, bool force = false);
    void set_edge_weight(graph& b, const graph::edge& e, bool directed = true, bool force = false);

    real_t get_edge_weight(const graph& g, natural_t from, natural_t to);
    real_t get_edge_weight(const graph& g, const graph::edge& e);

    bool has_cycle(const graph& g);

    bool edge_exists(const graph& g, natural_t from, natural_t to);
    bool edge_exists(const graph& g, const graph::edge& e);

    std::vector<graph::edge>::iterator get_edge(graph& g, natural_t from, natural_t to);

    void set_loop_weights(graph& g, real_t weight);

    std::vector<natural_t> topological_sort(const graph& g);

  }  // namespace graph
}  // namespace tensor

#endif  // TENSORLIB_GRAPH_GRAPH_HPP