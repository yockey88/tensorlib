/**
 * \file simulation/simulation_node.cpp
 **/
#include "simulation/simulation_node.hpp"

#include <iostream>
#include <print>

#include "core/serialization.hpp"

#include "simulation/session.hpp"

namespace tensor {
  namespace network {

    simulation_node::simulation_node(asio::io_context& io_ctx, natural_t id)
        : id(id), io_context(io_ctx), message_handler(this) {
      /// create socket control block
      // std::println("Simulation node {} created", id);
    }

    void simulation_node::activate(const binding_point control_endpoint) {
      control_binding_point = control_endpoint;
      asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4(control_endpoint.ip), control_endpoint.port);
      control_socket = make_owning_ptr<asio::ip::tcp::socket>(io_context);
      control_socket->async_connect(endpoint, std::bind_front(&simulation_node::on_control_connect, this));
    }

    void simulation_node::write_control_message(const std::vector<uint8_t>& message) {
      if (message.empty()) {
        std::print(std::cerr, "Control message is empty\n");
        return;
      }

      if (control_buffer.writing) {
        control_buffer.write_queue.push(message);
        return;
      }

      std::ranges::fill(control_buffer.write_buffer, 0);
      std::ranges::copy(message, control_buffer.write_buffer.begin());

      control_buffer.writing = true;
      control_socket->async_send(asio::buffer(control_buffer.write_buffer, message.size()), std::bind_front(&simulation_node::finish_control_write, this));
    }

    void simulation_node::on_control_connect(const asio::error_code& ec) {
      if (ec == asio::error::operation_aborted || ec == asio::error::already_connected) {
        return;
      }

      if (ec) {
        std::print(std::cerr, " !> Control connect error: {}\n", ec.message());
        return;
      }

      std::println("  > [node {}] control layer connection established : {}", id, control_binding_point);
      session_status_request status_request;
      session::session_type st;
      {
        /// status-type is 16 bit value, first 8 are CONTROL_CONNECTION because this is on the control socket
        ///  the second 8 are the session type (layer or node)
        st.type = session::CONTROL_CONNECTION;
        st.node_type = session::NODE_CONNECTION;
      }
      status_request.session_type = st.combined_type;
      // layer already has a node id so no need to ask for one
      status_request.node_id = id;
      status_request.layer_type = 0;  // bc we node

      std::println("  > [node {}] {}", id, core::as_string(status_request));
      write_control_message(status_request.build());
      message_handler.node_state = NODE_ACTIVE;

      control_buffer.reading = true;
      control_socket->async_receive(asio::buffer(control_buffer.read_buffer), std::bind_front(&simulation_node::finish_control_read, this));
    }

    void simulation_node::finish_control_write(const asio::error_code& ec, std::size_t bytes_transferred) {
      control_buffer.writing = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }

      if (ec) {
        std::print(std::cerr, "Control write error: {}\n", ec.message());
        return;
      }

      if (!control_buffer.write_queue.empty()) {
        auto message = control_buffer.write_queue.front();
        control_buffer.write_queue.pop();

        write_control_message(message);
      }
    }

    void simulation_node::finish_control_read(const asio::error_code& ec, std::size_t bytes_transferred) {
      control_buffer.reading = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof || bytes_transferred == 0) {
        return;
      }

      if (ec) {
        std::print(std::cerr, "Control read error: {}\n", ec.message());
        return;
      }

      std::vector<uint8_t> data;
      std::ranges::copy(control_buffer.read_buffer.begin(), control_buffer.read_buffer.begin() + bytes_transferred, std::back_inserter(data));
      std::ranges::fill(control_buffer.read_buffer, 0);
      handle_control_message(data);

      control_buffer.reading = true;
      control_socket->async_receive(asio::buffer(control_buffer.read_buffer), std::bind_front(&simulation_node::finish_control_read, this));
    }

    void simulation_node::handle_control_message(const std::vector<uint8_t>& data) {
      if (data.empty()) {
        std::print(std::cerr, "Control message is empty\n");
        return;
      }

      if (data.size() < 2) {
        /// put bytes into parsing buffer to wait for more data
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
        std::println(std::cerr, "Control message handler error: {}:{}", header.category, header.id);
      } catch (const std::exception& e) {
        std::println(std::cerr, "Control message handler error: {}", e.what());
      } catch (...) {
        std::println(std::cerr, "Control message handler unknown error");
      }
    }

    void simulation_node::handle_shutdown_request(const session_shutdown_request& msg) {
      TENSORLIB_ASSERT(control_socket != nullptr, "Control block is null");

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
      if (st.node_type != session::NODE_CONNECTION) {
        std::print(std::cerr, " !> Layer {} invalid node type: {:#06x}\n", id, st.combined_type);
        ack.ack_nack = 0;
        return;
      }

      std::println("  > [node {}] {}", id, core::as_string(ack));
      write_control_message(ack.build());
    }

  }  // namespace network
}  // namespace tensor