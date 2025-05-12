/**
 * \file simulation/simulation_control.cpp
 **/
#include "simulation/simulation_control.hpp"

namespace tensor {
  namespace network {

    void simulation_control::launch_nodes(const simulation_config& config) {
      network_graph = config.network;
      for (const auto& node : network_graph.nodes) {
        nodes[node.id] = make_owning_ptr<simulation_node>(*io_context, node.id);
      }
    }

  }  // namespace network
}  // namespace tensor