/**
 * \file simulation/node_message_handler.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_NODE_MESSAGE_HANDLER_HPP
#define TENSORLIB_NETWORK_SIMULATION_NODE_MESSAGE_HANDLER_HPP

#include "message/message.hpp"

namespace tensor {
  namespace network {

    class simulation_node;

    enum node_state_type {
      NODE_LAUNCHING = 0,

      NODE_ACTIVE,

      NODE_SHUTTING_DOWN,
      NODE_STOPPED,
    };

    class node_message_handler {
     public:
      node_message_handler(simulation_node* node) : node(node) {}
      ~node_message_handler() = default;

      void handle_message(const message& msg);

      node_state_type node_state = NODE_STOPPED;

     private:
      simulation_node* node;

      void handle_active_message(const message& msg);
      void handle_control_message(const message& msg);

      void handle_simulation_message(const message& msg);
      void handle_simulation_event_message(const message& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_NODE_MESSAGE_HANDLER_HPP