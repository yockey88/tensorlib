/**
 * \file simulation/session.cpp
 **/
#include "simulation/session.hpp"

#include <iostream>
#include <print>

#include <asio/error.hpp>

#include "core/serialization.hpp"

#include "message/message.hpp"
#include "session_message_handler.hpp"
#include "simulation/session_message_handler.hpp"
#include "simulation/simulation_control.hpp"
#include "simulation/simulation_layer.hpp"
#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    void session::start() {
      if (!socket.is_open()) {
        std::println(std::cerr, " !> [Session {}] socket is not open", id);
        return;
      }

      message_handler.session_state = SESSION_LAUNCHING;

      reading = true;
      socket.async_receive(asio::buffer(read_buffer, kBufferSize), std::bind_front(&session::handle_read, this));
    }

    void session::poll() {
    }

    void session::begin_shutdown() {
      session_shutdown_request msg;
      msg.session_type = session_info.combined_type;
      msg.node_id = node_id;
      msg.status = 0;  /// everything is ok
      std::println("  > [Session {}] {}", id, core::as_string(msg));
      write_message(msg.build());

      message_handler.session_state = SESSION_SHUTTING_DOWN;

      if (!reading) {
        socket.async_receive(asio::buffer(read_buffer, kBufferSize), std::bind_front(&session::handle_read, this));
      }
    }

    void session::stop() {
      socket.shutdown(asio::ip::tcp::socket::shutdown_both);
      socket.close();
    }

    void session::write_message(const std::vector<uint8_t>& message) {
      if (message.empty()) {
        std::print(std::cerr, " !> Session {} message is empty\n", id);
        return;
      }

      if (is_writing) {
        write_queue.push(message);
        return;
      }

      current_write_buffer = message;
      socket.async_send(asio::buffer(current_write_buffer, current_write_buffer.size()), std::bind_front(&session::handle_write, this));
      is_writing = true;
    }

    std::vector<uint8_t> session::read_message() {
      if (read_queue.empty()) {
        return {};
      }

      auto message = std::move(read_queue.front());
      read_queue.pop();

      return message;
    }

    void session::handle_read(const asio::error_code& ec, std::size_t bytes_transferred) {
      reading = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof || ec == asio::error::connection_reset) {
        return;
      }
      if (ec) {
        return;
      }
      if (bytes_transferred == 0) {
        std::print(std::cerr, " !> Session {} read 0 bytes\n", id);
        return;
      }

      std::vector<uint8_t> data;
      std::ranges::copy(read_buffer.begin(), read_buffer.begin() + bytes_transferred, std::back_inserter(data));
      handle_message(data);

      std::ranges::fill(read_buffer, 0);
      socket.async_receive(asio::buffer(read_buffer), std::bind_front(&session::handle_read, this));
    }

    void session::handle_write(const asio::error_code& ec, std::size_t bytes_transferred) {
      is_writing = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof || ec == asio::error::connection_reset) {
        return;
      }
      if (ec) {
        return;
      }

      if (!write_queue.empty()) {
        auto message = write_queue.front();
        write_queue.pop();
        write_message(message);
      }
    }

    void session::handle_message(const std::vector<uint8_t>& data) {
      if (data.empty()) {
        std::print(std::cerr, " !> Session {} message is empty\n", id);
        return;
      }
      if (data.size() < 2) {
        std::print(std::cerr, " !> Session {} message is too small\n", id);
        return;
      }

      message_header header = {
        .category = static_cast<message_category>(data[0]),
        .id = static_cast<message_id>(data[1]),
      };
      message msg(header, std::vector<uint8_t>(data.begin() + 2, data.end()));

      try {
        message_handler.handle_message(msg);
      } catch (const message& msg) {
        std::print(std::cerr, " !> Session {} message handler error: {}:{}", id, msg.header.category, msg.header.id);
      } catch (const std::exception& e) {
        std::print(std::cerr, " !> Session {} message handler error: {}\n", id, e.what());
      } catch (...) {
        std::print(std::cerr, " !> Session {} message handler unknown error\n", id);
      }
    }

    void session::handle_status_request(const session_status_request& msg) {
      /// always respond to the status request
      session_type st;
      st.combined_type = msg.session_type;

      /// 0 implies asking for the session type
      if (st.type == 0) {
        /// handle this when for sim nodes
      }

      if (st.node_type == session::LAYER_CONNECTION || st.node_type == session::NODE_CONNECTION) {
      } else {
        std::println(std::cerr, " !> Session {} invalid node type: {:#06x}", id, st.combined_type);
        /// nack
        return;
      }

      session_info.combined_type = st.combined_type;
      node_id = msg.node_id;

      session_status_response response;
      response.session_type = session_info.combined_type;
      response.node_id = node_id;

      /// TODO: get status bits
      response.status = 0;

      std::println("  > [Session {}] {}", id, core::as_string(response));
      write_message(response.build());
      message_handler.session_state = SESSION_ACTIVE;

      /// if we are active then are done
      /// otherwise if are starting up and a layer we send the simulation description
      if (message_handler.session_state != SESSION_LAUNCHING) {
        return;
      }

      if (st.node_type == session::LAYER_CONNECTION) {
        layer_type = static_cast<simulation_layer_type>(msg.layer_type);
        binding_point ep = sim_ctrl->sim_state->get_layer_endpoint(*layer_type);

        simulation_description sim_desc;
        sim_desc.session_type = session_info.combined_type;
        sim_desc.node_id = node_id;
        sim_desc.status = 0;  /// everything is ok
        sim_desc.simulation_binding_point = binding_point{
          .port = ep.port,
          .ip = ep.ip,
        };

        std::println("  > [Session {}] {}", id, core::as_string(sim_desc));
        write_message(sim_desc.build());
      } else if (st.node_type == session::NODE_CONNECTION) {
        binding_point comm_ep = sim_ctrl->sim_state->get_layer_endpoint(SIM_COMM_LAYER);
        binding_point analysis_ep = sim_ctrl->sim_state->get_layer_endpoint(SIM_ANALYSIS_LAYER);

        // sim_ctrl->register_node(node_id, ...);

        node_initialization_request node_init;
        node_init.session_type = session_info.combined_type;
        node_init.node_id = node_id;
        node_init.comm_layer_endpoint = comm_ep;
        node_init.analytics_layer_endpoint = analysis_ep;

        std::println("  > [Session {}] {}", id, core::as_string(node_init));
        // write_message(node_init.build());
      }
    }

  }  // namespace network
}  // namespace tensor