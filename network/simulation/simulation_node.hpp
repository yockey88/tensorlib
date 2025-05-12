/**
 * \file simulation/simulation_nodes.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_NODE_HPP
#define TENSORLIB_NETWORK_SIMULATION_NODE_HPP

#include <asio/asio.hpp>

#include "core/types.hpp"

namespace tensor {
  namespace network {

    class simulation_node {
     public:
      simulation_node(asio::io_context& io_ctx, natural_t id);
      ~simulation_node() = default;

     private:
      natural_t id;
      asio::io_context& io_context;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_NODE_HPP