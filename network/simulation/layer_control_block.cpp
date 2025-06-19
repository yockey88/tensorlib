/**
 * \file simulation/layer_control_block.cpp
 **/
#include "simulation/layer_control_block.hpp"

#include <iostream>
#include <print>

#include <flatbuffers/flexbuffers.h>

#include "core/serialization.hpp"

#include "message/message.hpp"
#include "simulation/session.hpp"
#include "simulation/simulation_layer.hpp"
#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    layer_control_block::layer_control_block(simulation_layer* layer)
        : layer(layer), message_handler(layer) {
      TENSORLIB_ASSERT(layer != nullptr, "Layer is null");
      id = layer->id;
    }

    void layer_control_block::bind(asio::io_context& io_ctx, const binding_point& endpoint) {
      asio::ip::tcp::endpoint ctrl_ep(asio::ip::make_address_v4(endpoint.ip), endpoint.port);
      control_socket = make_owning_ptr<asio::ip::tcp::socket>(io_ctx);
      control_socket->async_connect(ctrl_ep, std::bind_front(&layer_control_block::on_control_connect, this));
    }

    void layer_control_block::listen_for_nodes(const binding_point& binding_point, opt<natural_t> num_nodes) {
      if (binding_point.port == 0) {
        std::print(std::cerr, "Failed to bind layer endpoint: port is 0\n");
        return;
      }

      asio::ip::tcp::endpoint sim_ep(asio::ip::make_address_v4(binding_point.ip), binding_point.port);
      node_acceptor = make_owning_ptr<asio::ip::tcp::acceptor>(layer->get_io_context(), sim_ep);
      node_acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
      if (num_nodes.has_value()) {
        num_expected_nodes = *num_nodes;
      }

      std::println("  > [node {}] listening for nodes on {}", id, binding_point);
      node_acceptor->async_accept(std::bind_front(&layer_control_block::on_simulation_connect, this));
    }

    void layer_control_block::shutdown() {
      if (control_socket != nullptr) {
        control_socket->shutdown(asio::ip::tcp::socket::shutdown_both);
        control_socket->close();
      }
      control_socket = nullptr;
    }

    void layer_control_block::write_control_message(const std::vector<uint8_t>& message) {
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
      control_socket->async_send(asio::buffer(control_buffer.write_buffer, message.size()), std::bind_front(&layer_control_block::finish_control_write, this));
      control_buffer.writing = true;
    }

    opt<std::vector<uint8_t>> layer_control_block::read_control_message() {
      // if (control_buffer.read_queue.empty()) {
      //   return std::nullopt;
      // }

      // auto msg = std::move(control_buffer.read_queue.front());
      // control_buffer.read_queue.pop();
      // return msg;
      return {};
    }

    void layer_control_block::on_control_connect(const asio::error_code& ec) {
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }

      if (ec) {
        return;
      }

      control_binding_point = binding_point::from_asio(control_socket->remote_endpoint().address(), control_socket->remote_endpoint().port());
      std::println("  > [Layer {}] layer control connection established : {}", id, control_binding_point);

      session_status_request status_request;
      session::session_type st;
      {
        /// status-type is 16 bit value, first 8 are CONTROL_CONNECTION because this is on the control socket
        ///  the second 8 are the session type (layer or node)
        st.type = session::CONTROL_CONNECTION;
        st.node_type = session::LAYER_CONNECTION;
      }
      status_request.session_type = st.combined_type;
      // layer already has a node id so no need to ask for one
      status_request.node_id = id;
      status_request.layer_type = layer->get_layer_type();

      std::println("  > [layer {}] {}", id, core::as_string(status_request));
      write_control_message(status_request.build());

      message_handler.layer_state = LAYER_ACTIVE;

      control_buffer.reading = true;
      control_socket->async_receive(asio::buffer(control_buffer.read_buffer), std::bind_front(&layer_control_block::finsh_control_read, this));

      flags.control_connected = true;
      if (flags.simulation_connected) {
        flags.node_configured = true;
        message_handler.layer_state = LAYER_ACTIVE;
      }
    }

    void layer_control_block::finsh_control_read(const asio::error_code& ec, std::size_t bytes_transferred) {
      control_buffer.reading = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }

      if (ec) {
        control_buffer.reading = true;
        control_socket->async_receive(asio::buffer(control_buffer.read_buffer), std::bind_front(&layer_control_block::finsh_control_read, this));
        return;
      }

      std::vector<uint8_t> data;
      std::ranges::copy(control_buffer.read_buffer.begin(), control_buffer.read_buffer.begin() + bytes_transferred, std::back_inserter(data));
      std::ranges::fill(control_buffer.read_buffer, 0);
      parse_message(data, session::CONTROL_CONNECTION);

      control_buffer.reading = true;
      control_socket->async_receive(asio::buffer(control_buffer.read_buffer), std::bind_front(&layer_control_block::finsh_control_read, this));
    }

    void layer_control_block::finish_control_write(const asio::error_code& ec, std::size_t bytes_transferred) {
      control_buffer.writing = false;
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }

      if (ec) {
        return;
      }

      if (!control_buffer.write_queue.empty()) {
        auto message = control_buffer.write_queue.front();
        control_buffer.write_queue.pop();

        write_control_message(message);
      }
    }

    void layer_control_block::on_simulation_connect(const asio::error_code& ec, asio::ip::tcp::socket socket) {
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }
      if (ec) {
        std::print(std::cerr, "Failed to accept node connection: {}\n", ec.message());
        return;
      }
      if (num_expected_nodes == 0) {
        /// send error to session
        return;
      }

      node_connection node_conn(num_expected_nodes, this);
      node_conn.id = num_expected_nodes;
      node_conn.simulation_buffer = read_write_buffer();
      node_conn.socket = make_owning_ptr<asio::ip::tcp::socket>(std::move(socket));
      node_conn.socket->async_receive(asio::buffer(node_conn.simulation_buffer.read_buffer), std::bind_front(&layer_control_block::node_connection::finish_simulation_read, &node_conn));

      auto [itr, success] = node_connections.insert({ node_conn.id, std::move(node_conn) });
      if (!success) {
        std::print(std::cerr, "Failed to insert node connection into map: {}\n", node_conn.id);
        return;
      }

      std::println("  > [node {}] connection established : {}:{}", node_conn.id, itr->second.socket->remote_endpoint().address().to_string(), itr->second.socket->remote_endpoint().port());
      if (num_expected_nodes > 0) {
        --num_expected_nodes;
      }
    }

    void layer_control_block::node_connection::write_to_node(const std::vector<uint8_t>& message) {
      if (message.empty()) {
        std::print(std::cerr, "Node message is empty\n");
        return;
      }

      if (simulation_buffer.writing) {
        simulation_buffer.write_queue.push(message);
        return;
      }

      std::ranges::fill(simulation_buffer.write_buffer, 0);
      std::ranges::copy(message, simulation_buffer.write_buffer.begin());
      socket->async_send(asio::buffer(simulation_buffer.write_buffer, message.size()), std::bind_front(&layer_control_block::node_connection::finish_simulation_write, this));
      simulation_buffer.writing = true;
    }

    std::vector<uint8_t> layer_control_block::node_connection::read_from_node() {
      // if (simulation_buffer.read_queue.empty()) {
      //   return {};
      // }

      // auto msg = std::move(simulation_buffer.read_queue.front());
      // simulation_buffer.read_queue.pop();
      // return msg;
      return {};
    }

    void layer_control_block::node_connection::finish_simulation_read(const asio::error_code& ec, std::size_t bytes_transferred) {
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }
      if (ec) {
        std::print(std::cerr, "Simulation read error: {}\n", ec.message());
        return;
      }

      if (bytes_transferred == 0) {
        std::print(std::cerr, "Simulation read 0 bytes\n");
        return;
      }

      std::vector<uint8_t> data;
      std::ranges::copy(simulation_buffer.read_buffer.begin(), simulation_buffer.read_buffer.begin() + bytes_transferred, std::back_inserter(data));
      std::ranges::fill(simulation_buffer.read_buffer, 0);
      std::print("  > [node {}] simulation message: {}\n", id, core::as_string(data));
      control_block->handle_node_message(data);
    }

    void layer_control_block::node_connection::finish_simulation_write(const asio::error_code& ec, std::size_t bytes_transferred) {
      if (ec == asio::error::operation_aborted || ec == asio::error::eof) {
        return;
      }
      if (ec) {
        std::print(std::cerr, "Simulation write error: {}\n", ec.message());
        return;
      }

      if (bytes_transferred == 0) {
        std::print(std::cerr, "Simulation write 0 bytes\n");
        return;
      }

      if (!simulation_buffer.write_queue.empty()) {
        auto message = simulation_buffer.write_queue.front();
        simulation_buffer.write_queue.pop();

        // control_block->write_control_message(message);
      }
    }

    void layer_control_block::parse_message(const std::vector<uint8_t>& data, uint8_t conn_type) {
      session::connection_type type = static_cast<session::connection_type>(conn_type);
      if (data.empty()) {
        std::println(std::cerr, "Control message data is empty");
        return;
      }

      if (data.size() < 2) {
        /// put bytes into parsing buffer to wait for more data
        return;
      }

      if (type == session::CONTROL_CONNECTION) {
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
      } else if (type == session::SIMULATION_CONNECTION) {
      }
    }

    void layer_control_block::handle_node_message(const std::vector<uint8_t>& msg) {}

  }  // namespace network
}  // namespace tensor