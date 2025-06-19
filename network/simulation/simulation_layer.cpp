/**
 * \file simulation/simulation_layer
 **/
#include "simulation/simulation_layer.hpp"

#include <sstream>
#include <string>

#include <flatbuffers/flexbuffers.h>

#include "core/serialization.hpp"
#include "core/types.hpp"

#include "message/message.hpp"
#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    simulation_layer::simulation_layer(simulation_state* sim_state, simulation_layer_type type)
        : io_context(sim_state->get_io_context()), type(type), heartbeat_timer(sim_state->get_io_context()) {
    }

    std::string session_endpoint::write_string(const session_endpoint& endpoint) {
      std::stringstream ss;
      ss << "Simulation endpoint: ";
      for (integer_t i = 3; i >= 0; --i) {
        ss << std::to_string(endpoint.simulation.bytes[i]);
        if (i != 0) {
          ss << ".";
        }
      }
      ss << ":" << endpoint.simulation.port << " Control endpoint: ";
      for (integer_t i = 3; i >= 0; --i) {
        ss << std::to_string(endpoint.control.bytes[i]);
        if (i != 0) {
          ss << ".";
        }
      }
      ss << ":" << endpoint.control.port;
      return ss.str();
    }

    void simulation_layer::bind_control_endpoint(asio::io_context& io_ctx, const binding_point& endpoint) {
      if (endpoint.port == 0) {
        std::print(std::cerr, "Failed to bind layer endpoint: port is 0\n");
        return;
      }

      control_block->bind(io_ctx, endpoint);
    }

    void simulation_layer::shutdown() {
      if (control_block != nullptr) {
        //// send shutdown messages to other nodes/layers???
        control_block->shutdown();
        control_block = nullptr;
        on_shutdown();
      }
    }

    void simulation_layer::start_heartbeat_timer() {
      heartbeat_timer.expires_after(std::chrono::seconds(1));
      heartbeat_timer.async_wait(std::bind_front(&simulation_layer::on_heartbeat_timeout, this));
    }

    void simulation_layer::on_heartbeat_timeout(const asio::error_code& ec) {
      if (ec == asio::error::operation_aborted) {
        return;
      }

      if (ec) {
        std::print(std::cerr, "Heartbeat timer error: {}\n", ec.message());
        return;
      }

      session_status_request msg;
      msg.session_type = session_info;
      msg.node_id = id;

      std::println("  > [Layer {}] {}", id, core::as_string(msg));
      control_block->write_control_message(msg.build());

      heartbeat_timer.expires_after(std::chrono::seconds(1));
      heartbeat_timer.async_wait(std::bind_front(&simulation_layer::on_heartbeat_timeout, this));
    }

    void simulation_layer::handle_simulation_description(const simulation_description& msg) {
      TENSORLIB_ASSERT(control_block != nullptr, "Control block is null");

      std::println("  > [Layer {}] {}", id, core::as_string(msg));
      if (msg.session_type != session_info) {
        std::print(std::cerr, " !> Layer {} invalid session type: {:#06x}\n", id, msg.session_type);
        return;
      }

      if (msg.node_id != id) {
        std::print(std::cerr, " !> Layer {} invalid node id: {}\n", id, msg.node_id);
        return;
      }

      binding_point sim_bp = msg.simulation_binding_point;
      control_block->listen_for_nodes(sim_bp);
    }

    void simulation_layer::handle_shutdown_request(const session_shutdown_request& msg) {
      TENSORLIB_ASSERT(control_block != nullptr, "Control block is null");

      heartbeat_timer.cancel();

      acknowledgement ack;
      ack.acked_header = {
        .category = msg.category,
        .id = msg.id,
      };
      ack.node_id = id;

      session::session_type st;
      st.combined_type = msg.session_type;
      if (st.type != session::CONTROL_CONNECTION) {
        std::print(std::cerr, " !> Layer {} invalid session type: {:#06x}\n", id, st.combined_type);
        ack.ack_nack = 0;
        return;
      }
      if (st.node_type != session::LAYER_CONNECTION) {
        std::print(std::cerr, " !> Layer {} invalid node type: {:#06x}\n", id, st.combined_type);
        ack.ack_nack = 0;
        return;
      }

      std::println("  > [Layer {}] {}", id, core::as_string(ack));
      control_block->write_control_message(ack.build());
    }

  }  // namespace network
}  // namespace tensor