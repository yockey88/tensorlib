/**
 * \file network/network.cpp
 **/
#include <filesystem>
#include <iostream>
#include <print>
#include <thread>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "simulation/simulation.hpp"

int main() {
  std::println("Running network simulation in {}", std::filesystem::current_path().string());
#if 1
  tensor::opt<tensor::owning_ptr<tensor::network::simulation>> sim_res = tensor::network::simulation::load_from_file("network/simulations/test_sim1.bin");
  if (!sim_res) {
    std::println(std::cerr, "Failed to load simulation from file");
    return -1;
  }

  std::println("Simulation loaded successfully");
  try {
    tensor::owning_ptr<tensor::network::simulation> sim = std::move(*sim_res);
    sim->initialize();

    /// let simulation run in background
    do {
      std::this_thread::yield();
    } while (sim->running());

    sim->shutdown();
    std::println("Simulation complete");
  } catch (const std::exception& e) {
    std::println(std::cerr, " !> Simulation error: {}", e.what());
    return -1;
  }

#else
  asio::io_context io_context;
  /// spawn an executor to asyncrhonously read from stdin

  std::jthread io_thread([&](std::stop_token stoken) {
    do {
      try {
        io_context.run();
        if (stoken.stop_requested()) {
          io_context.stop();
        } else {
          io_context.restart();
        }
      } catch (const std::exception& e) {
        std::print(std::cerr, "Error in input system: {}\n", e.what());
        continue;
      }
    } while (!stoken.stop_requested());
  });

  std::atomic<bool> waiting = true;
  asio::signal_set signals(io_context, SIGINT, SIGTERM);  // catch Ctrl+C and termination signals
  signals.async_wait([&](const asio::error_code& error, int signal_number) {
    if (!error) {
      if (signal_number == SIGINT || signal_number == SIGTERM) {
        std::print("Received shutdown signal: {}\n", signal_number);
        waiting = false;
      } else {
        std::print("Received unknown signal: {}\n", signal_number);
      }
    } else {
      std::print(std::cerr, "Error receiving signal: {}\n", error.message());
    }
  });

  while (waiting) {
    std::this_thread::yield();
  }
  std::print("Received signal, stopping io_context...\n");

  //// process stdin input
  // asio::async_initiate(io_context, [&](const asio::error_code& error) {
  //   std::print("Reading from stdin...\n");

  //   if (!std::cin.good()) {
  //     std::print(std::cerr, "Error reading from stdin\n");
  //     return;
  //   }
  //   std::string input;
  //   if (std::getline(std::cin, input)) {
  //     std::lock_guard lock(io_mutex);
  //     stdin_buffer += input + "\n";
  //     std::print("Read from stdin: {}\n", input);
  //   }
  // });

  io_thread.request_stop();
  if (io_thread.joinable()) {
    io_thread.join();
  }
#endif

  std::println("Exit successful");
  return 0;
}