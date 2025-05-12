/**
 * \file simulation/simulation_node.cpp
 **/
#include "simulation/simulation_node.hpp"

#include <print>

namespace tensor {
  namespace network {

    simulation_node::simulation_node(asio::io_context& io_ctx, natural_t id)
        : id(id), io_context(io_ctx) {
      /// create socket control block
      // std::println("Simulation node {} created", id);
    }

  }  // namespace network
}  // namespace tensor