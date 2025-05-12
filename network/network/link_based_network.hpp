/**
 * \file network/link_based_network.hpp
 **/
#ifndef TENSORLIB_NETWORK_LINK_BASED_NETWORK_HPP
#define TENSORLIB_NETWORK_LINK_BASED_NETWORK_HPP

#include "core/types.hpp"

#include "graph/graph.hpp"

namespace tensor {
  namespace network {

    struct link_based_network {
      graph::graph g;

      static opt<link_based_network> load_from_file(const std::string& filename);
      static void write_to_file(const std::string& filename, const link_based_network& network);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_LINK_BASED_NETWORK_HPP