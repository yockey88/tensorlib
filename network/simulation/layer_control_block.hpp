/**
 * \file simulation/layer_control_block.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_LAYER_CONTROL_BLOCK_HPP
#define TENSORLIB_NETWORK_SIMULATION_LAYER_CONTROL_BLOCK_HPP

#include <array>
#include <map>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "message/message.hpp"
#include "simulation/layer_message_handler.hpp"
#include "simulation/read_write_buffer.hpp"

namespace tensor {
  namespace network {

    class layer_control_block {
     public:
      layer_control_block(simulation_layer* layer);
      ~layer_control_block() = default;

      void bind(asio::io_context& io_ctx, const binding_point& endpoint);
      void listen_for_nodes(const binding_point& binding_point, opt<natural_t> num_nodes = std::nullopt);
      void shutdown();

      void write_control_message(const std::vector<uint8_t>& message);

      opt<std::vector<uint8_t>> read_control_message();

     private:
      simulation_layer* layer;
      layer_message_handler message_handler;

      friend class simulation_layer;
      struct state_flags {
        bool control_connected = false;
        bool simulation_connected = false;

        bool node_configured = false;
      };
      state_flags flags;

      natural_t id = 0;
      uint32_t num_expected_nodes = 0;

      /// \todo: clean this up
      binding_point control_binding_point;
      read_write_buffer control_buffer;
      owning_ptr<asio::ip::tcp::socket> control_socket = nullptr;

      struct node_connection {
        natural_t id = 0;
        layer_control_block* control_block = nullptr;

        read_write_buffer simulation_buffer;
        owning_ptr<asio::ip::tcp::socket> socket = nullptr;

        void write_to_node(const std::vector<uint8_t>& message);
        std::vector<uint8_t> read_from_node();

        void finish_simulation_read(const asio::error_code& ec, std::size_t bytes_transferred);
        void finish_simulation_write(const asio::error_code& ec, std::size_t bytes_transferred);

        node_connection(natural_t id, layer_control_block* control_block)
            : id(id), control_block(control_block) {}
      };
      owning_ptr<asio::ip::tcp::acceptor> node_acceptor = nullptr;
      std::map<natural_t, node_connection> node_connections;

      void on_control_connect(const asio::error_code& ec);
      void finsh_control_read(const asio::error_code& ec, std::size_t bytes_transferred);
      void finish_control_write(const asio::error_code& ec, std::size_t bytes_transferred);

      void on_simulation_connect(const asio::error_code& ec, asio::ip::tcp::socket socket);

      void parse_message(const std::vector<uint8_t>& data, uint8_t conn_type);
      void handle_node_message(const std::vector<uint8_t>& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_LAYER_CONTROL_BLOCK_HPP