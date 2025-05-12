/**
 * \file simulation/simulation_config.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP
#define TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP

#include <filesystem>
#include <string>
#include <vector>

#include "core/types.hpp"

#include "graph/graph.hpp"
#include "simulation_layer.hpp"

namespace tensor {
  namespace network {

    enum simulation_type {
      LINK_BASED_NETWORK = 0,
    };

    struct simulation_config {
      std::string name;
      std::string description;

      simulation_type type = LINK_BASED_NETWORK;

      /// simulation parameters
      graph::graph network;
      uint16_t main_step_interval = 1000;  // in milliseconds

      /// simulation control
      /// event information
      struct layer_data {
        simulation_layer_type type;
        std::filesystem::path path;
      };
      std::vector<layer_data> layers;

      static opt<simulation_config> load_from_file(const std::string& filename);
      static void write_to_file(const std::string& filename, const simulation_config& config);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP