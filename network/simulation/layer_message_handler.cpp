/**
 * \file simulation/layer_message_handler.cpp
 **/
#include "simulation/layer_message_handler.hpp"

#include <iostream>
#include <print>

#include "message/message.hpp"
#include "simulation/simulation_layer.hpp"

namespace tensor {
  namespace network {

    void layer_message_handler::handle_message(const message& msg) {
      switch (layer_state) {
        case LAYER_LAUNCHING:
        case LAYER_SHUTTING_DOWN:
          handle_control_message(msg);
          break;

        case LAYER_ACTIVE:
          handle_active_message(msg);
          break;

        case LAYER_STOPPED:
          break;
      }
    }

    void layer_message_handler::handle_active_message(const message& msg) {
      switch (msg.header.category) {
        case CONTROL:
          handle_control_message(msg);
          break;

        case SIMULATION:
          handle_simulation_message(msg);
          break;

        case SIMULATION_EVENT:
          handle_simulation_event_message(msg);
          break;

        default:
          throw msg;
      }
    }

    void layer_message_handler::handle_control_message(const message& msg) {
      switch (msg.header.id) {
        case PONG: {
          session_status_response response = session_status_response::parse(msg.data);
          if (response.node_id != layer->id) {
            std::print(std::cerr, " !> Layer {} invalid node id: {}\n", layer->id, response.node_id);
            return;
          }
          layer->session_info = response.session_type;

          // layer->start_heartbeat_timer();
          layer_state = LAYER_ACTIVE;
        } break;

        case SIM_DESCRIPTION:
          layer->handle_simulation_description(simulation_description::parse(msg.data));
          break;

        case NODE_SHUTDOWN_REQUEST:
          layer->handle_shutdown_request(session_shutdown_request::parse(msg.data));
          break;

        default:
          throw msg;
      }
    }

    void layer_message_handler::handle_simulation_message(const message& msg) {
      switch (msg.header.id) {
        case SIM_EVENT:
          handle_simulation_event_message(msg);
          break;

        default:
          throw msg;
      }
    }

    void layer_message_handler::handle_simulation_event_message(const message& msg) {
    }

  }  // namespace network
}  // namespace tensor