/**
 * \file simulation/node_message_handler.cpp
 **/
#include "simulation/node_message_handler.hpp"

#include <iostream>
#include <print>

#include "message/message.hpp"
#include "node_message_handler.hpp"
#include "simulation/simulation_node.hpp"

namespace tensor {
  namespace network {

    void node_message_handler::handle_message(const message& msg) {
      switch (node_state) {
        case NODE_LAUNCHING:
        case NODE_SHUTTING_DOWN:
          handle_control_message(msg);
          break;

        case NODE_ACTIVE:
          handle_active_message(msg);
          break;

        case NODE_STOPPED:
          break;
      }
    }

    void node_message_handler::handle_active_message(const message& msg) {
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

    void node_message_handler::handle_control_message(const message& msg) {
      switch (msg.header.id) {
        case PONG: {
          session_status_response response = session_status_response::parse(msg.data);
          if (response.node_id != node->id) {
            std::print(std::cerr, " !> Node {} invalid node id: {}\n", node->id, response.node_id);
            return;
          }
          node->session_info = response.session_type;
          node_state = NODE_ACTIVE;
        } break;

        case SIM_DESCRIPTION:
          break;

        case NODE_SHUTDOWN_REQUEST:
          node->handle_shutdown_request(session_shutdown_request::parse(msg.data));
          break;

        default:
          throw msg;
      }
    }

    void node_message_handler::handle_simulation_message(const message& msg) {
      switch (msg.header.id) {
        case SIM_EVENT:
          handle_simulation_event_message(msg);
          break;

        default:
          throw msg;
      }
    }

    void node_message_handler::handle_simulation_event_message(const message& msg) {
    }

  }  // namespace network
}  // namespace tensor