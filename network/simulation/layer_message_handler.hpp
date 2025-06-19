/**
 * \file layer_message_handler.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_LAYER_MESSAGE_HANDLER_HPP
#define TENSORLIB_NETWORK_SIMULATION_LAYER_MESSAGE_HANDLER_HPP

#include "message/message.hpp"

namespace tensor {
  namespace network {

    class simulation_layer;

    enum layer_state_type {
      LAYER_LAUNCHING = 0,

      LAYER_ACTIVE,

      LAYER_SHUTTING_DOWN,
      LAYER_STOPPED,
    };

    class layer_message_handler {
     public:
      layer_message_handler(simulation_layer* layer)
          : layer(layer) {}

      void handle_message(const message& msg);

      layer_state_type layer_state = LAYER_STOPPED;

     private:
      simulation_layer* layer = nullptr;

      void handle_active_message(const message& msg);

      void handle_control_message(const message& msg);
      void handle_simulation_message(const message& msg);
      void handle_simulation_event_message(const message& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_LAYER_MESSAGE_HANDLER_HPP