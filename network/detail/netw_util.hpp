/**
 * \file detail/util.hpp
 **/
#ifndef TENSORLIB_NETWORK_DETAIL_NETW_UTIL_HPP
#define TENSORLIB_NETWORK_DETAIL_NETW_UTIL_HPP

#include <flatbuffers/flatbuffers.h>

#include "fb_specs/network_spec_generated.h"
#include "graph/graph.hpp"

namespace tnet_config = tensor::network::config;

namespace tensor {
  namespace detail {

    graph::graph read_graph_from_flatbuffer(const tnet_config::graph* config);

  }  // namespace detail
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_DETAIL_NETW_UTIL_HPP