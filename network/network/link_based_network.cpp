/**
 * \file network/link_based_network.cpp
 **/
#include "network/link_based_network.hpp"

#include <fstream>
#include <iostream>
#include <print>

#include "detail/netw_util.hpp"
#include "detail/util.hpp"

#include "fb_specs/network_spec_generated.h"

namespace tnet_config = tensor::network::config;

namespace tensor {
  namespace network {

    opt<link_based_network> link_based_network::load_from_file(const std::string& filename) {
      std::vector<uint8_t> buffer = detail::read_to_bytes(filename);
      if (buffer.empty()) {
        std::print(std::cerr, "Failed to read file: {}\n", filename);
        return std::nullopt;
      }

      const tnet_config::network* config = tnet_config::Getnetwork(buffer.data());
      if (config == nullptr) {
        std::print(std::cerr, "Failed to parse network config from file: {}\n", filename);
        return std::nullopt;
      }

      const tnet_config::graph* graph_config = config->connections();
      if (graph_config == nullptr) {
        std::print(std::cerr, "Failed to parse graph config from file: {}\n", filename);
        return std::nullopt;
      }

      link_based_network netw;
      netw.g = detail::read_graph_from_flatbuffer(graph_config);

      std::print("Loaded network with {} nodes and {} edges\n", netw.g.nodes.size(), netw.g.num_edges);
      return netw;
    }

    static void write_to_file(const std::string& filename, const link_based_network& network) {
      std::ofstream file{ filename + tnet_config::networkIdentifier(), std::ios::binary | std::ios::out };
      if (!file.is_open()) {
        return;
      }

      // auto builder = flatbuffers::FlatBufferBuilder();
      // auto nodes = std::vector<flatbuffers::Offset<uint64_t>>();
      // for (const auto& node : network.g.nodes) {
      //   nodes.push_back(builder.CreateString(std::to_string(node.id)));
      // }

      // auto edges = std::vector<flatbuffers::Offset<tnet_config::edge>>();
      // for (const auto& node_edges : network.g.edges) {
      //   for (const auto& edge : node_edges) {
      //     edges.push_back(tnet_config::Createedge(builder, edge.from, edge.to, edge.weight));
      //   }
      // }
      // auto edges_vector = builder.CreateVector(edges);

      // auto graph = tnet_config::Creategraph(builder, network.g.nodes.size(), network.g.num_edges, edges_vector);
      return;
    }

  }  // namespace network
}  // namespace tensor
