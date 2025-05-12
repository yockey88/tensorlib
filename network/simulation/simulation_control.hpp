/**
 * \file simulation/simulation_control.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP
#define TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP

#include <map>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "graph/graph.hpp"
#include "simulation/simulation_config.hpp"
#include "simulation/simulation_node.hpp"

namespace tensor {
  namespace network {

    class simulation_control {
     public:
      simulation_control(owning_ptr<asio::io_context>& io_context)
          : io_context(io_context) {}
      ~simulation_control() = default;

      void launch_nodes(const simulation_config& config);

     private:
      graph::graph network_graph;
      std::map<natural_t, owning_ptr<simulation_node>> nodes;

      owning_ptr<asio::io_context>& io_context;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP