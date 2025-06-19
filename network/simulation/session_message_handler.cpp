/**
 * \file simulation/session_message_handler.cpp
 **/
#include "simulation/session_message_handler.hpp"

#include <print>

#include "message/message.hpp"
#include "simulation/session.hpp"
#include "simulation/simulation_control.hpp"

namespace tensor {
  namespace network {

    void session_message_handler::handle_message(const message& msg) {
      TENSORLIB_ASSERT(session != nullptr, "Session is null");
      switch (session_state) {
        case SESSION_LAUNCHING:
        case SESSION_SHUTTING_DOWN:
          handle_control_message(msg);
          break;

        case SESSION_ACTIVE:
          handle_active_message(msg);
          break;

        case SESSION_STOPPED:
          break;
      }
    }

    void session_message_handler::handle_active_message(const message& msg) {
      switch (msg.header.category) {
        case ACKNOWLEDGEMENT:
        case CONTROL:
          handle_control_message(msg);
          break;

        case SIMULATION:
          break;

        case SIMULATION_EVENT:
          break;

        default:
          throw msg;
      }
    }

    void session_message_handler::handle_control_message(const message& msg) {
      TENSORLIB_ASSERT(session != nullptr, "Session is null");

      switch (msg.header.id) {
        case PING:
          /// session status request
          session->handle_status_request(session_status_request::parse(msg.data));
          break;

        case PONG:
          break;

        case ACK:
          /// check whether ack/nack, for now we shut down regardless
          if (session->message_handler.session_state == SESSION_SHUTTING_DOWN) {
            session->sim_ctrl->alert_session_shutdown(session->id);
          }
          break;

        default:
          break;
      }
    }

    void session_message_handler::handle_simulation_message(const message& msg) {
      TENSORLIB_ASSERT(session != nullptr, "Session is null");
      TENSORLIB_ASSERT(msg.header.category == SIMULATION, "Invalid message category for simulation message");

      switch (msg.header.id) {
        default:
          break;
      }
    }

    void session_message_handler::handle_simulation_event_message(const message& msg) {
      TENSORLIB_ASSERT(session != nullptr, "Session is null");
      TENSORLIB_ASSERT(msg.header.category == SIMULATION_EVENT, "Invalid message category for simulation event message");

      switch (msg.header.id) {
        case SIM_EVENT:
          break;

        default:
          break;
      }
    }

  }  // namespace network
}  // namespace tensor