/**
 * \file detail/netw_util.cpp
 **/
#include "detail/netw_util.hpp"

#include <iostream>

namespace tensor {
  namespace detail {

    graph::graph read_graph_from_flatbuffer(const tnet_config::graph* config) {
      graph::graph g;

      const flatbuffers::Vector<natural_t>* nodes = config->nodes();
      g.nodes.resize(nodes->size());
      g.edges.resize(nodes->size());

      for (natural_t i = 0; i < nodes->size(); ++i) {
        g.nodes[i].id = nodes->Get(i);
        g.edges[i] = std::vector<graph::graph::edge>();
      }

      const flatbuffers::Vector<flatbuffers::Offset<tnet_config::edge>>* edges = config->edges();
      for (natural_t i = 0; i < edges->size(); ++i) {
        const tnet_config::edge* edge = edges->Get(i);
        if (edge == nullptr) {
          std::print(std::cerr, "Failed to parse edge config from file\n");
          continue;
        }
        graph::graph::edge e = { edge->from(), edge->to(), static_cast<real_t>(edge->weight()) };
        g.edges[edge->from()].emplace_back(e);
        g.num_edges++;

        if (edge->from() == edge->to()) {
          g.has_loops = true;
        }
      }

      return g;
    }

  }  // namespace detail
}  // namespace tensor