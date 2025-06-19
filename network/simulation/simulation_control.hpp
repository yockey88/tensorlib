/**
 * \file simulation/simulation_control.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP
#define TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP

#include <map>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "graph/graph.hpp"
#include "simulation/session.hpp"
#include "simulation/simulation_config.hpp"
#include "simulation/simulation_node.hpp"

namespace tensor {
  namespace network {

    class simulation_state;

    class simulation_control {
     public:
      simulation_control(simulation_state* sim_state, owning_ptr<asio::io_context>& io_context)
          : sim_state(sim_state), io_context(io_context),
            control_acceptor(*io_context, asio::ip::tcp::endpoint(asio::ip::address(), simulation_config::kControlPort)) {}
      ~simulation_control() = default;

      const simulation_config& get_config() const;

      void launch_nodes(const simulation_config& config);
      void activate_nodes(const std::vector<natural_t>& node_ids);

      void network_launch();

      void poll();

      void shutdown_nodes();
      bool nodes_cleaned_up();

      void shutdown();

      natural_t get_next_node_id();

      void alert_session_shutdown(natural_t id);

      simulation_state* sim_state = nullptr;

     private:
      bool shutting_down = false;

      graph::graph network_graph;
      std::map<natural_t, owning_ptr<simulation_node>> nodes;

      owning_ptr<asio::io_context>& io_context;

      asio::ip::tcp::acceptor control_acceptor;

      struct connection {
        natural_t id;
        owning_ptr<session> session;
      };
      std::map<natural_t, connection> connections;
      std::queue<natural_t> dead_connections_queue;
      uint32_t closed_nodes = 0;

      natural_t get_next_conn_id() {
        static std::atomic<natural_t> conn_id = 0;
        return ++conn_id;
      }

      /// control_block control;
      /// simulation_block sim;

      void open_channel();
      void handle_accept(const asio::error_code& ec, asio::ip::tcp::socket socket);
      void finalize_connection(asio::ip::tcp::socket socket, session::connection_type type);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_CONTROL_HPP