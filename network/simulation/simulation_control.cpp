/**
 * \file simulation/simulation_control.cpp
 **/
#include "simulation/simulation_control.hpp"

#include <iostream>
#include <print>

#include "simulation/session_message_handler.hpp"
#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    const simulation_config& simulation_control::get_config() const {
      return sim_state->get_config();
    }

    void simulation_control::launch_nodes(const simulation_config& config) {
      open_channel();

      network_graph = config.network;
      for (const auto& node : network_graph.nodes) {
        nodes[node.id] = make_owning_ptr<simulation_node>(*io_context, node.id);
      }
    }

    void simulation_control::activate_nodes(const std::vector<natural_t>& node_ids) {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");

      for (const auto& id : node_ids) {
        auto itr = nodes.find(id);
        if (itr != nodes.end()) {
          /// activate node
          itr->second->activate(sim_state->get_control_endpoint());
        } else {
          std::print(std::cerr, " !> Node {} not found\n", id);
        }
      }
    }

    void simulation_control::poll() {
      if (dead_connections_queue.size() > 0) {
        while (dead_connections_queue.size() > 0) {
          auto _ = dead_connections_queue.front();
          dead_connections_queue.pop();

          /// handle other node closing down things

          closed_nodes++;
        }
      }

      if (shutting_down) {
        return;
      }
    }

    void simulation_control::shutdown_nodes() {
      for (auto& [id, conn] : connections) {
        if (conn.session != nullptr) {
          conn.session->begin_shutdown();
        }
      }
    }

    bool simulation_control::nodes_cleaned_up() {
      return closed_nodes == connections.size();
    }

    void simulation_control::shutdown() {
      for (auto& control_conn : connections) {
        control_conn.second.session->stop();
      }

      // for (auto& node : nodes) {
      //   node.second->stop();
      // }
      control_acceptor.close();
    }

    natural_t simulation_control::get_next_node_id() {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");
      static std::atomic<integer_t> node_id = -1;
      if (node_id < 0) {
        node_id = sim_state->get_config().network.nodes.size();
      }
      return node_id++;
    }

    void simulation_control::alert_session_shutdown(natural_t id) {
      std::println("  => [SIM-CTRL] session {} closed", id);
      dead_connections_queue.push(id);
    }

    void simulation_control::open_channel() {
      control_acceptor.async_accept(std::bind_front(&simulation_control::handle_accept, this));
    }

    void simulation_control::handle_accept(const asio::error_code& ec, asio::ip::tcp::socket socket) {
      if (ec == asio::error::operation_aborted) {
        return;
      }
      if (ec) {
        std::print(std::cerr, " !> Control accept error: {}\n", ec.message());
        return;
      }

      finalize_connection(std::move(socket), session::CONTROL_CONNECTION);
      open_channel();
    }

    void simulation_control::finalize_connection(asio::ip::tcp::socket socket, session::connection_type type) {
      connection conn;
      conn.id = get_next_conn_id();

      auto [itr, success] = connections.insert({ conn.id, std::move(conn) });
      if (!success) {
        std::print(std::cerr, " !> Simulation connection already exists: {}\n", conn.id);
        conn.session->stop();
        conn.session = nullptr;
        return;
      }

      std::println("  => [SIM-CTRL] connection [{}] established [{}]", conn.id, type);
      itr->second.session = make_owning_ptr<session>(this, std::move(socket), itr->second.id, type);
      itr->second.session->start();

      /// make this a little cleaner
      if (sim_state->get_current_state() == simulation_state_type::SIMULATION_LAUNCHING &&
          connections.size() == sim_state->layer_map.size()) {
        /// send simulation start event
      }
    }

  }  // namespace network
}  // namespace tensor