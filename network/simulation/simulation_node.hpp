/**
 * \file simulation/simulation_nodes.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_NODE_HPP
#define TENSORLIB_NETWORK_SIMULATION_NODE_HPP

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "message/message.hpp"
#include "simulation/node_message_handler.hpp"
#include "simulation/read_write_buffer.hpp"

namespace tensor {
  namespace network {

    class simulation_node {
     public:
      simulation_node(asio::io_context& io_ctx, natural_t id);
      virtual ~simulation_node() = default;

      void activate(const binding_point control_endpoint);

      void write_control_message(const std::vector<uint8_t>& message);

     private:
      natural_t id;
      asio::io_context& io_context;

      uint16_t session_info = 0;
      binding_point control_binding_point;
      owning_ptr<asio::ip::tcp::socket> control_socket;

      read_write_buffer control_buffer;
      read_write_buffer sim_buffer;

      friend class node_message_handler;
      node_message_handler message_handler;

      void on_control_connect(const asio::error_code& ec);
      void finish_control_write(const asio::error_code& ec, std::size_t bytes_transferred);
      void finish_control_read(const asio::error_code& ec, std::size_t bytes_transferred);

      void handle_control_message(const std::vector<uint8_t>& data);

      void handle_shutdown_request(const session_shutdown_request& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_NODE_HPP