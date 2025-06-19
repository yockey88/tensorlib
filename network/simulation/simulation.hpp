/**
 * \file simulation/simulation.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_HPP
#define TENSORLIB_NETWORK_SIMULATION_HPP

#include <iostream>
#include <type_traits>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "simulation/simulation_config.hpp"
#include "simulation/simulation_event_handler.hpp"
#include "simulation/simulation_layer.hpp"
#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    // struct system {
    //   owning_ptr<simulation_system> system_ptr = nullptr;
    //   std::mutex system_mutex;
    // };

    class simulation {
     public:
      simulation() {}
      ~simulation() {}

      simulation_state& get_simulation_state();

      bool running() const;

      void load_simulation(const opt<simulation_config>& config);

      void initialize();

      void shutdown();

      static opt<owning_ptr<simulation>> load_from_file(const std::string& filename);

     private:
      simulation_config sim_config;

      /// simulation control
      ///   > sim_state is main simulation running sim thread in the background
      owning_ptr<simulation_state> sim_state;

      std::mutex event_mtx;
      owning_ptr<simulation_event_handler> event_handler = nullptr;

      /// asio control
      owning_ptr<asio::signal_set> signals = nullptr;
      owning_ptr<asio::io_context> io_context = nullptr;
      std::jthread io_thread;

      void signal_handler(const asio::error_code& ec, int signal_number);

      void bind_control_events();
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_HPP