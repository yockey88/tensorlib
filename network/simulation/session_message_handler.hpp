/**
 * \file simulation/session_message_handler.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_SESSION_MESSAGE_HANDLER_HPP
#define TENSORLIB_NETWORK_SIMULATION_SESSION_MESSAGE_HANDLER_HPP

#include "message/message.hpp"

namespace tensor {
  namespace network {

    class session;

    enum session_state_type {
      SESSION_LAUNCHING = 0,

      SESSION_ACTIVE,

      SESSION_SHUTTING_DOWN,
      SESSION_STOPPED,
    };
    enum session_state_event {
    };

    class session_message_handler {
     public:
      session_message_handler(session* session)
          : session(session) {}
      ~session_message_handler() = default;

      void handle_message(const message& msg);

      session_state_type session_state = SESSION_STOPPED;

     private:
      session* session = nullptr;

      void handle_active_message(const message& msg);

      void handle_control_message(const message& msg);
      void handle_simulation_message(const message& msg);
      void handle_simulation_event_message(const message& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_SESSION_MESSAGE_HANDLER_HPP