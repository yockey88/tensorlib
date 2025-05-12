/**
 * \file simulation/simulation_state.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_STATE_HPP
#define TENSORLIB_NETWORK_SIMULATION_STATE_HPP

#include <mutex>
#include <print>
#include <thread>
#include <vector>

#include "message/message.hpp"
#include "simulation/dyn_library.hpp"
#include "simulation/library_loader.hpp"
#include "simulation/simulation_config.hpp"
#include "simulation/simulation_control.hpp"
#include "simulation/simulation_event.hpp"
#include "simulation/simulation_event_handler.hpp"
#include "simulation/simulation_layer.hpp"
#include "simulation/simulation_node.hpp"
#include "simulation/system.hpp"

namespace tensor {
  namespace network {

    enum simulation_state_type {
      SIMULATION_LAUNCHING = 0,

      SIMULATION_WAITING,
      SIMULATION_PROCESSING,

      SIMULATION_SHUTTING_DOWN,
      SIMULATION_STOPPED,
    };

    class simulation_state : simulation_system {
     public:
      simulation_state(owning_ptr<simulation_event_handler>& event_handler, std::mutex& event_mtx, owning_ptr<asio::io_context>& io_context, const simulation_config& config)
          : sim_control(make_owning_ptr<simulation_control>(io_context)), event_mtx(event_mtx), event_handler(event_handler), config(config) {}
      ~simulation_state() = default;

      const simulation_config& get_config() const { return config; }
      simulation_state_type get_current_state() const {
        return current_state;
      }
      bool simulation_in_state(simulation_state_type state) const;

      void bind_layer(simulation_layer* layer);

      void launch();
      void stop();
      void cleanup();

      void handle_event(const simulation_event& event);

      void handle_message(const message& msg);

     private:
      std::mutex state_mutex;
      simulation_state_type current_state = SIMULATION_STOPPED;

      struct state_flags {
        /// mixed used
        std::atomic<bool> running = false;
        std::atomic<bool> finalized = false;

        std::atomic<bool> error_occurred = false;

        /// thread used
        bool initialized = false;
      } checkpoints;

      struct threadlocal_data {
        owning_ptr<message_channel> tx_channel;
        owning_ptr<message_channel> rx_channel;
        std::stop_token stoken;
      };

      static thread_local threadlocal_data* thread_data;

      std::jthread simulation_thread;

      owning_ptr<simulation_control> sim_control;
      std::map<uint64_t, lib_ptr> layer_libraries;

      struct sim_layer {
        simulation_layer* layer = nullptr;
        simulation_layer_type type = SIM_EVENT_LAYER;
        symbol delete_fn;
      };
      std::map<uint64_t, sim_layer> layer_map;

      std::mutex& event_mtx;
      owning_ptr<simulation_event_handler>& event_handler;

      simulation_config config;

      std::vector<simulation_node> nodes;
      std::vector<simulation_event> events;
      std::vector<simulation_layer*> layers;

      owning_ptr<message_channel> tx_channel;
      owning_ptr<message_channel> rx_channel;

      /// main thread functions only
      void wait_for_ack();

      //// mixed used functions that must use mutex
      void set_current_state(simulation_state_type state, bool override = false);

      opt<integer_t> exit_code = std::nullopt;
      void simulation_main(owning_ptr<message_channel>&& thread_tx_channel, owning_ptr<message_channel>&& thread_rx_channel, std::stop_token stoken);
      void do_main_timestep();

      //// simulation thread functions only
      void wait_for_initialization();
      void handle_init_msg(const message& msg);
      void bind_layer(const std::filesystem::path& path, simulation_layer_type type);

      void wait_for_start();
      void handle_start_msg(const message& msg);

      void wait_for_shutdown();
      void handle_shutdown_msg(const message& msg);

      void handle_simulation_event(const message& msg);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_STATE_HPP
