/**
 * \file simulation/simulation_config.cpp
 **/
#include "simulation/simulation_config.hpp"

#include <iostream>
#include <print>

#include "core/serialization.hpp"
#include "detail/netw_util.hpp"
#include "detail/util.hpp"

#include "fb_specs/simulation_config_generated.h"
#include "fb_specs/simulation_event_generated.h"
#include "simulation_event.hpp"
#include "simulation_layer.hpp"

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

      /// layer information
      if (config->layers() != nullptr) {
        const ::flatbuffers::Vector<::flatbuffers::Offset<tnet_sim_cfg::layer_description>>* raw_layers = config->layers();
        if (raw_layers == nullptr) {
          std::print(std::cerr, "Failed to get layers from simulation config\n");
          return std::nullopt;
        }
        sim_config.layers.reserve(raw_layers->size());
        for (natural_t i = 0; i < raw_layers->size(); ++i) {
          const tnet_sim_cfg::layer_description* layer = raw_layers->Get(i);
          if (layer == nullptr) {
            std::print(std::cerr, "Failed to get layer from simulation config\n");
            return std::nullopt;
          }
          sim_config.layers.push_back({
            .path = layer->path()->str(),
            .node_ids = std::vector<natural_t>{ layer->nodes()->begin(), layer->nodes()->end() },
          });
        }
      }

      /// get binding points
      if (config->shim_endpoint() != nullptr) {
        sim_config.process_endpoint(config->shim_endpoint(), SIM_ANALYSIS_LAYER);
      }
      if (config->comm_endpoint() != nullptr) {
        std::println("  [note] comm endpoint found in simulation config");
        sim_config.process_endpoint(config->comm_endpoint(), SIM_COMM_LAYER);
      }

      if (!sim_config.analysis_session_endpoint.has_value()) {
        sim_config.analysis_session_endpoint = kTensorEnvEndpoint;
      }
      if (!sim_config.comm_session_endpoint.has_value()) {
        sim_config.comm_session_endpoint = kTensorEnvEndpoint;
      }

      /// events information
      sim_config.process_events(config->events());

      std::println("loaded simulation config: {} : {}", sim_config.name, sim_config.description);
      std::println("  > main step interval: {} ms", sim_config.main_step_interval);
      std::println("  > network:\n{}", core::as_string(sim_config.network));
      std::println("  > comm layer endpoint: [{}]", core::as_string(*sim_config.comm_session_endpoint));
      std::println("  > analysis layer endpoint: [{}]", core::as_string(*sim_config.analysis_session_endpoint));
      std::println("\n");

      return sim_config;
    }

    void simulation_config::write_to_file(const std::string& filename, const simulation_config& config) {
    }

    void simulation_config::process_endpoint(const tnet_sim::binding_point* endpoint, simulation_layer_type layer_type) {
      if (endpoint == nullptr) {
        std::print(std::cerr, "Failed to get comm endpoint from simulation config\n");
        return;
      }
      std::println("> processing endpoint description for layer [{}]", type);

      binding_point ep;
      ep.port = endpoint->port();
      const tensor::simulation::ip_bytes* bytes = endpoint->ip_as_ip_bytes();
      if (bytes != nullptr) {
        for (natural_t i = 0; i < 4; ++i) {
          ep.bytes[i] = bytes->bytes()->Get(i);
        }
      } else {
        /// localhost
        ep.ip = 0x7f000001;
      }

      switch (layer_type) {
        case SIM_ANALYSIS_LAYER: {
          analysis_session_endpoint = ep;
        } break;

        case SIM_COMM_LAYER: {
          comm_session_endpoint = ep;
        } break;
      }
    }

    void simulation_config::process_events(const ::flatbuffers::Vector<::flatbuffers::Offset<tensor::simulation::event>>* events) {
      TENSORLIB_ASSERT(events != nullptr, "Failed to get events from simulation events");

      for (natural_t i = 0; i < events->size(); ++i) {
        const tensor::simulation::event* event = events->Get(i);
        if (event == nullptr) {
          std::print(std::cerr, "Failed to get event from simulation config\n");
          return;
        }

        event_key key;
        key.id = event->id().value_or(simulation_event::generate_event_id());

        std::vector<natural_t> node_ids;
        if (event->nodes() != nullptr) {
          node_ids.reserve(event->nodes()->size());
          for (natural_t j = 0; j < event->nodes()->size(); ++j) {
            node_ids.push_back(event->nodes()->Get(j));
          }
        }

        switch (event->desc_type()) {
          case tnet_sim::event_description_builtin_event:
            process_builtin_event(key, event->desc_as_builtin_event(), std::move(node_ids));
            break;

          case tnet_sim::event_description_user_event:
            process_user_event(key, event->desc_as_user_event(), std::move(node_ids));
            break;

          default:
            std::print("  > unknown event type {}\n", event->desc_type());
            break;
        }
        // event_desc
      }
    }

    void simulation_config::process_builtin_event(event_key key, const tnet_sim::builtin_event* event, std::vector<natural_t> node_ids) {
      simulation_event sim_event;

      switch (event->data_type()) {
        case tnet_sim::event_data_node_activation_event: {
          const tnet_sim::node_activation_event* node_event = event->data_as_node_activation_event();
          if (node_event == nullptr) {
            std::print(std::cerr, "Failed to get node activation event from simulation config\n");
            return;
          }

          sim_event.type = NODE_EVENT_ACTIVATE;

          switch (node_event->time_type()) {
            case tnet_sim::event_time_time_block: {
              const tnet_sim::time_block* time_block = node_event->time_as_time_block();
              TENSORLIB_ASSERT(time_block != nullptr, "Failed to get time block from node activation event");

              sim_event.time.min_step = time_block->start();
              sim_event.time.max_step = time_block->stop();
              key.interval = time_block->interval();
            } break;

            case tnet_sim::event_time_time_point: {
              const tnet_sim::time_point* time_point = node_event->time_as_time_point();
              TENSORLIB_ASSERT(time_point != nullptr, "Failed to get time point from node activation event");
              sim_event.time = {
                .min_step = time_point->time(),
                .max_step = time_point->time(),
              };
              /// todo: check  for duration
              key.interval = 0;
            } break;

            default:
              std::print("  > unknown event time type {}\n", node_event->time_type());
              break;
          }
        } break;

        default:
          std::print("  > unknown builtin event type {}\n", event->data_type());
          break;
      }

      std::ranges::copy(node_ids, std::back_inserter(sim_event.node_ids));
      key.time = sim_event.time;
      auto [itr, success] = events.insert({ key, std::move(sim_event) });
      if (!success || itr == events.end()) {
        std::print(std::cerr, "Failed to insert event into map: {} : {}\n", key.id, key.interval);
        return;
      }
      std::print("{}", simulation_event::write_string(itr->second));
    }

    void simulation_config::process_user_event(event_key key, const tnet_sim::user_event* event, std::vector<natural_t> node_ids) {}

  }  // namespace network
}  // namespace tensor