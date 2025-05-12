/**
 * \file simulation/simulation.cpp
 **/
#include "simulation/simulation.hpp"

#include <chrono>
#include <iostream>
#include <print>

#include <asio/asio.hpp>

#include "simulation/library_loader.hpp"
#include "simulation/simulation_config.hpp"
#include "simulation/simulation_state.hpp"
#include "simulation_event.hpp"

namespace tensor {
  namespace network {
    namespace detail {
      namespace {

        void io_main(asio::io_context& io_context, std::stop_token stoken) {
          do {
            try {
              asio::executor_work_guard<asio::io_context::executor_type> work = asio::make_work_guard(io_context);
              io_context.run();
              if (!stoken.stop_requested()) {
                io_context.restart();
              }
            } catch (const std::exception& e) {
              std::print(std::cerr, " !> IO error: {}\n", e.what());
            }
          } while (!stoken.stop_requested());
        }

      }  // namespace
    }  // namespace detail

    simulation_state& simulation::get_simulation_state() {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");
      return *sim_state.get();
    }

    bool simulation::running() const {
      if (sim_state == nullptr) {
        return false;
      }

      return sim_state->simulation_in_state(SIMULATION_WAITING) ||
        sim_state->simulation_in_state(SIMULATION_PROCESSING);
    }

    void simulation::load_simulation(const opt<simulation_config>& config) {
      if (!config) {
        std::print(std::cerr, "Failed to load simulation config\n");
        return;
      }
      sim_config = *config;

      io_context = make_owning_ptr<asio::io_context>();
      event_handler = make_owning_ptr<simulation_event_handler>(*io_context);

      sim_state = make_owning_ptr<simulation_state>(event_handler, event_mtx, io_context, sim_config);
    }

    void simulation::initialize() {
      if (sim_state == nullptr) {
        throw std::runtime_error("No simulation loaded!");
      }

      library_loader::initialize_platform();

      /// load events from config
      bind_signal_handlers();
      bind_control_events();

      io_thread = std::jthread([&](std::stop_token stoken) {
        detail::io_main(*io_context, stoken);
      });

      //// after launch the simulation controller will be running its main controller loop
      sim_state->launch();
    }

    void simulation::shutdown() {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");

      sim_state->cleanup();

      io_context->stop();
      io_thread.request_stop();
      if (io_thread.joinable()) {
        io_thread.join();
      }

      library_loader::shutdown_platform();

      signals = nullptr;
      io_context = nullptr;

      event_handler = nullptr;
      sim_state = nullptr;
    }

    opt<owning_ptr<simulation>> simulation::load_from_file(const std::string& filename) {
      opt<simulation_config> config = simulation_config::load_from_file(filename);
      if (!config) {
        std::print(std::cerr, "Failed to load simulation config from file: {}\n", filename);
        return std::nullopt;
      }

      owning_ptr<simulation> sim = make_owning_ptr<simulation>();
      sim->load_simulation(config);

      return sim;
    }

    void simulation::bind_signal_handlers() {
      signals = make_owning_ptr<asio::signal_set>(*io_context, SIGINT, SIGTERM);
      signals->async_wait(std::bind_front(&simulation::signal_handler, this));
    }

    void simulation::signal_handler(const asio::error_code& ec, int signal_number) {
      if (ec) {
        std::print(std::cerr, " !> Signal error: {}\n", ec.message());
        bind_signal_handlers();
      } else {
        std::print("Received signal: {}\n", signal_number);
        sim_state->handle_event(sim_event(SIM_EVENT_REQUEST_STOP));
      }
    }

    void simulation::bind_control_events() {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");
      TENSORLIB_ASSERT(event_handler != nullptr, "Event handler is null");
      TENSORLIB_ASSERT(io_context != nullptr, "IO context is null");

      ///  no simulation thread yet so no need to lock
      event_handler->set_main_step_interval(
        std::chrono::milliseconds(sim_config.main_step_interval), [this]() {
          sim_state->handle_event(sim_event(SIM_EVENT_MAIN_STEP));
        }
      );
    }

  }  // namespace network
}  // namespace tensor