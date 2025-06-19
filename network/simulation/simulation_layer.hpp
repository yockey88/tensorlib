/**
 * \file simulation/simulation_layer.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_LAYER_HPP
#define TENSORLIB_NETWORK_SIMULATION_LAYER_HPP

#include <asio/asio.hpp>

#include "core/defines.hpp"
#include "core/owning_ptr.hpp"
#include "core/types.hpp"

#include "asio/steady_timer.hpp"
#include "message/message.hpp"
#include "simulation/layer_control_block.hpp"
#include "simulation/simulation_event.hpp"

namespace tensor {
  namespace network {

    class simulation_state;

    class simulation_state;

    enum simulation_layer_type : uint8_t {
      /// INVALID = 0
      SIM_ANALYSIS_LAYER = 1,
      SIM_COMM_LAYER,
    };

    class TENSORCLASS simulation_layer {
     public:
      simulation_layer(simulation_state* sim_state, simulation_layer_type type);
      virtual ~simulation_layer() = default;

      virtual simulation_layer_type get_layer_type() = 0;

      virtual void initialize() {}
      void bind_control_endpoint(asio::io_context& io_ctx, const binding_point& endpoint);

      virtual void begin_timestep() {}
      virtual void update() {}
      virtual void end_timestep() {}

      void shutdown();
      virtual void on_shutdown() {}

      virtual void handle_event(const simulation_event& event) {}

      natural_t id = 0;
      uint16_t session_info;
      owning_ptr<layer_control_block> control_block = nullptr;

     protected:
      asio::io_context& get_io_context() { return io_context; }

      virtual void on_receive_control_message(const message& data) {}
      virtual void on_receive_simulation_message(const message& data) {}

     private:
      asio::io_context& io_context;
      simulation_layer_type type;

      asio::steady_timer heartbeat_timer;

      friend class layer_control_block;
      friend class layer_message_handler;

      void start_heartbeat_timer();
      void on_heartbeat_timeout(const asio::error_code& ec);

      void handle_simulation_description(const simulation_description& msg);
      void handle_shutdown_request(const session_shutdown_request& msg);
    };

  }  // namespace network
}  // namespace tensor

#define SIM_LAYER(name, type, ...)                                              \
  name(tensor::network::simulation_state* sim_state __VA_OPT__(, ) __VA_ARGS__) \
      : simulation_layer(sim_state, tensor::network::type) {}

#define TENSOR_SIM_LAYER(name)                                                                                     \
  TENSORAPI inline tensor::network::simulation_layer* create_layer(tensor::network::simulation_state* sim_state) { \
    return new name(sim_state);                                                                                    \
  }                                                                                                                \
  TENSORAPI inline void delete_layer(tensor::network::simulation_layer* layer) {                                   \
    delete layer;                                                                                                  \
  }

#endif  // TENSORLIB_NETWORK_SIMULATION_LAYER_HPP