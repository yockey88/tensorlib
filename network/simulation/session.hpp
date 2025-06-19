/**
 * \file simulation/session.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_SESSION_HPP
#define TENSORLIB_NETWORK_SIMULATION_SESSION_HPP

#include <cstdint>
#include <queue>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "message/message.hpp"
#include "simulation/session_message_handler.hpp"
#include "simulation/simulation_layer.hpp"

namespace tensor {
  namespace network {

    class simulation_control;

    class session {
     public:
      enum connection_type : uint8_t {
        CONTROL_CONNECTION,
        SIMULATION_CONNECTION,

        NODE_CONNECTION,
        LAYER_CONNECTION,
      };
#pragma pack(push, 1)
      struct session_type {
        union {
          struct {
            connection_type type;
            connection_type node_type;
          };
          uint16_t combined_type;
        };
      };
      static_assert(sizeof(session_type) == sizeof(uint16_t), "session_type size mismatch");
#pragma pack(pop)
      session(simulation_control* sim_ctrl, asio::ip::tcp::socket socket, natural_t id, connection_type type)
          : id(id), type(type), sim_ctrl(sim_ctrl), socket(std::move(socket)), message_handler(this) {}
      ~session() = default;

      void start();

      void poll();

      void begin_shutdown();
      void stop();

      void write_message(const std::vector<uint8_t>& message);
      std::vector<uint8_t> read_message();

      natural_t id;
      natural_t node_id = 0;

      connection_type type;
      session_type session_info;
      opt<simulation_layer_type> layer_type = std::nullopt;

     private:
      simulation_control* sim_ctrl;
      asio::ip::tcp::socket socket;

      friend class session_message_handler;
      session_message_handler message_handler;

      constexpr static size_t kBufferSize = 2048;

      bool reading = false;
      std::array<uint8_t, kBufferSize> read_buffer = { 0 };
      std::queue<std::vector<uint8_t>> read_queue;

      bool is_writing = false;
      std::vector<uint8_t> current_write_buffer = { 0 };
      std::queue<std::vector<uint8_t>> write_queue;

      void handle_read(const asio::error_code& ec, std::size_t bytes_transferred);
      void handle_write(const asio::error_code& ec, std::size_t bytes_transferred);

      void handle_message(const std::vector<uint8_t>& data);

      void handle_status_request(const session_status_request& data);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_SESSION_HPP