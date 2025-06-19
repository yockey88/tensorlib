/**
 * \file simulation/simulation_config.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP
#define TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP

#include <filesystem>
#include <string>
#include <vector>

#include "core/types.hpp"

#include "fb_specs/binding_point_generated.h"
#include "fb_specs/simulation_config_generated.h"
#include "graph/graph.hpp"
#include "simulation/simulation_layer.hpp"

namespace tnet_sim_cfg = tensor::simulation::config;
namespace tnet_sim = tensor::simulation;

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
      struct layer_description {
        std::filesystem::path path;
        std::vector<natural_t> node_ids;
      };
      std::vector<layer_description> layers = {};

      std::map<event_key, simulation_event> events = {};

      opt<binding_point> analysis_session_endpoint = std::nullopt;
      opt<binding_point> comm_session_endpoint = std::nullopt;

      constexpr static uint16_t kControlPort = 0xAA4A;
      constexpr static binding_point kTensorEnvEndpoint = {
        kControlPort,
        { 0x7f000001 },  // localhost
      };

      static opt<simulation_config> load_from_file(const std::string& filename);
      static void write_to_file(const std::string& filename, const simulation_config& config);

     private:
      void process_endpoint(const tnet_sim::binding_point* endpoint, simulation_layer_type type);
      void process_events(const ::flatbuffers::Vector<::flatbuffers::Offset<tensor::simulation::event>>* events);

      void process_builtin_event(event_key key, const tnet_sim::builtin_event* event, std::vector<natural_t> node_ids);
      void process_user_event(event_key key, const tnet_sim::user_event* event, std::vector<natural_t> node_ids);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_CONFIG_HPP