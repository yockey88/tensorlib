/**
 * \file simulation/simulation_config.cpp
 **/
#include "simulation/simulation_config.hpp"

#include <iostream>
#include <print>

#include "core/serialization.hpp"
#include "detail/netw_util.hpp"
#include "detail/util.hpp"

#include "fb_specs/simulation_config_spec_generated.h"

namespace tnet_sim_cfg = tensor::simulation::config;
namespace tnet_sim = tensor::simulation;

namespace tensor {
  namespace network {

    opt<simulation_config> simulation_config::load_from_file(const std::string& filename) {
      std::vector<uint8_t> buffer = detail::read_to_bytes(filename);
      if (buffer.empty()) {
        std::print(std::cerr, "Failed to read file: {}\n", filename);
        return std::nullopt;
      }

      const tnet_sim_cfg::simulation* config = tnet_sim_cfg::Getsimulation(buffer.data());
      if (config == nullptr) {
        std::print(std::cerr, "Failed to parse simulation config from file: {}\n", filename);
        return std::nullopt;
      }

      simulation_config sim_config;
      /// get main metadata
      sim_config.name = config->name()->str();
      sim_config.description = config->description()->str();
      sim_config.type = static_cast<simulation_type>(config->type());

      /// main simulation parameters
      const tnet_sim_cfg::parameters* params = config->sim_params();
      if (params != nullptr) {
        sim_config.network = detail::read_graph_from_flatbuffer(params->initial_network()->connections());
        sim_config.main_step_interval = params->main_step_interval();
        // other params
      } else {
        /// get default parameters
        // sim_config.network = graph::random_graph(10, 0.5);
        sim_config.main_step_interval = 1000;  // default 1 second
      }

      /// events information

      /// layer information
      if (config->layers() != nullptr) {
        const ::flatbuffers::Vector<::flatbuffers::Offset<tnet_sim::layer>>* raw_layers = config->layers();

        sim_config.layers.resize(raw_layers->size());
        for (natural_t i = 0; i < raw_layers->size(); ++i) {
          const tnet_sim::layer* layer = raw_layers->Get(i);
          sim_config.layers[i].type = static_cast<simulation_layer_type>(layer->type());
          sim_config.layers[i].path = layer->path()->str();
        }
      }

      std::println("loaded simulation config: {} : {}", sim_config.name, sim_config.description);
      std::println("  > main step interval: {} ms", sim_config.main_step_interval);
      std::println("  > network: {}", core::as_string(sim_config.network));
      return sim_config;
    }

    void simulation_config::write_to_file(const std::string& filename, const simulation_config& config) {
    }

  }  // namespace network
}  // namespace tensor