/**
 * \file simulation/simulation_state.cpp
 **/
#include "simulation/simulation_state.hpp"

#include <chrono>

#include <flatbuffers/flexbuffers.h>

#include "core/serialization.hpp"

#include "message/channel.hpp"
#include "message/message.hpp"
#include "simulation/simulation_event.hpp"

namespace tensor {
  namespace network {

    thread_local simulation_state::threadlocal_data* simulation_state::thread_data = nullptr;

    bool simulation_state::simulation_in_state(simulation_state_type state) const {
      return state == current_state;
    }

    void simulation_state::bind_layer(simulation_layer* layer) {
      // TENSORLIB_ASSERT(layer != nullptr, "Layer is null");
      // TENSORLIB_ASSERT(layer->get_simulation_state() == nullptr, "Layer already bound to a simulation state");

      // layer->set_simulation_state(this);
      // layers.push_back(layer);
    }

    void simulation_state::launch() {
      current_state = SIMULATION_WAITING;

      simulation_thread = std::jthread([&](std::stop_token stoken) {
        cref<channel_queue<message>> tx_queue = make_cref<channel_queue<message>>();
        cref<channel_queue<message>> rx_queue = make_cref<channel_queue<message>>();

        auto [this_tx_channel, sim_thread_rx_channel] = channel<message>::make_channel(tx_queue);
        auto [sim_thread_tx_channel, this_rx_channel] = channel<message>::make_channel(rx_queue);
        {
          std::lock_guard lock(state_mutex);
          this->tx_channel = std::move(this_tx_channel);
          this->rx_channel = std::move(this_rx_channel);
        }

        simulation_main(std::move(sim_thread_tx_channel), std::move(sim_thread_rx_channel), stoken);
        set_current_state(SIMULATION_STOPPED);
      });
      while (simulation_in_state(SIMULATION_WAITING)) {
        std::this_thread::yield();
      }

      TENSORLIB_ASSERT(tx_channel != nullptr, "Simulation thread tx channel is null");
      TENSORLIB_ASSERT(rx_channel != nullptr, "Simulation thread rx channel is null");

      {
        message init_msg;
        init_msg.category = CONTROL;
        init_msg.type = MSGID_THREAD_INITIALIZE;
        tx_channel->push(std::move(init_msg));
        wait_for_ack();
      }

      if (checkpoints.error_occurred) {
        std::println("Simulation thread launch error");
        return;
      }

      {
        message start_msg;
        start_msg.category = CONTROL;
        start_msg.type = MSGID_THREAD_START;
        tx_channel->push(std::move(start_msg));
        /// no ack for start
      }
    }

    void simulation_state::stop() {
      simulation_thread.request_stop();
    }

    void simulation_state::cleanup() {
      //// if we enter shutdown by other means then through main, ask the simulation to stop
      ///     then wait for it to finish
      if (simulation_in_state(SIMULATION_WAITING) || simulation_in_state(SIMULATION_PROCESSING)) {
        handle_event(sim_event(SIM_EVENT_REQUEST_STOP));
        while (!simulation_in_state(SIMULATION_SHUTTING_DOWN)) {
          std::this_thread::yield();
        }
      }

      {
        message shutdown_msg;
        shutdown_msg.category = CONTROL;
        shutdown_msg.type = MSGID_THREAD_SHUTDOWN;
        tx_channel->push(std::move(shutdown_msg));
        wait_for_ack();
      }

      /// let the thread exit
      while (!simulation_in_state(SIMULATION_STOPPED)) {
        std::this_thread::yield();
      }
      if (simulation_thread.joinable()) {
        simulation_thread.join();
      }

      rx_channel = nullptr;
      tx_channel = nullptr;
    }

    /// main thread event handling
    void simulation_state::handle_event(const simulation_event& event) {
      switch (event.type) {
        /// main thread events
        case SIM_EVENT_REQUEST_STOP:
          stop();
          break;

        /// in almost every case we queue for simulation thread
        default:
          TENSORLIB_ASSERT(tx_channel != nullptr, "Simulation thread tx channel is null");
          {
            message event_msg = event.get_message();
            event_msg.category = SIMULATION_EVENT;
            tx_channel->push(std::move(event_msg));
          }
          break;
      }
    }

    /// simulation thread message handling
    void simulation_state::handle_message(const message& msg) {
      switch (msg.category) {
        case NOTIFICATION:
          break;

        case CONTROL:
          break;

        case COMMAND:
          break;

        case QUERY:
          break;

        case RESPONSE:
          break;

        case ACKNOWLEDGEMENT:
          break;

        case ERROR_ALERT:
          break;

        case INFO:
          break;

        case SIMULATION:
          break;

        case SIMULATION_EVENT:
          handle_simulation_event(msg);
          break;
      }
    }

    void simulation_state::wait_for_ack() {
      TENSORLIB_ASSERT(tx_channel != nullptr, "Simulation thread tx channel is null");
      do {
        opt<message> msg = rx_channel->await_message(std::chrono::milliseconds(10));
        if (!msg) {
          continue;
        }

        if (msg->category == ACKNOWLEDGEMENT) {
          if (msg->type == MSGID_ACK) {
          } else if (msg->type == MSGID_NACK) {
            checkpoints.error_occurred = true;
            exit_code = -1;
          }
          break;
        } else if (msg->category == ERROR_ALERT) {
          checkpoints.error_occurred = true;
          exit_code = -1;
          break;
        }
      } while (!checkpoints.error_occurred);
    }

    void simulation_state::set_current_state(simulation_state_type state, bool override) {
      std::lock_guard lock(state_mutex);
      current_state = state;
    }

    void simulation_state::simulation_main(owning_ptr<message_channel>&& thread_tx_channel, owning_ptr<message_channel>&& thread_rx_channel, std::stop_token stoken) {
      set_current_state(SIMULATION_LAUNCHING);

      threadlocal_data threadlocal_data;
      {
        /// set up thread local data
        threadlocal_data.tx_channel = std::move(thread_tx_channel);
        threadlocal_data.rx_channel = std::move(thread_rx_channel);
        threadlocal_data.stoken = stoken;
        thread_data = &threadlocal_data;
      }

      wait_for_initialization();
      wait_for_start();

      {
        std::lock_guard lock(event_mtx);
        event_handler->start_all();
      }

      do {
        /// waiting means state is no longer transitioning, so simulation_state::launch() is unblocked
        set_current_state(SIMULATION_WAITING);
        opt<message> msg = thread_data->rx_channel->await_message(std::chrono::milliseconds(10));
        if (!msg) {
          continue;
        }

        set_current_state(SIMULATION_PROCESSING);
        handle_message(*msg);
      } while (checkpoints.running && !stoken.stop_requested() && !checkpoints.error_occurred);

      {
        std::lock_guard lock(event_mtx);
        event_handler->stop_all();
      }

      set_current_state(SIMULATION_SHUTTING_DOWN);
      if (checkpoints.error_occurred) {
        /// do something with errors, report them, attempt recovery?, etc...
      }
      wait_for_shutdown();
    }

    void simulation_state::do_main_timestep() {
      for (auto& layer : layer_map) {
        // layer.second.layer->begin_timestep();
      }

      {
        std::lock_guard lock(event_mtx);
        // event_handler->flush_events();
      }

      for (auto& layer : layer_map) {
        // layer.second.layer->update();
      }

      for (auto& layer : layer_map) {
        // layer.second.layer->end_timestep();
      }
    }

    void simulation_state::wait_for_initialization() {
      TENSORLIB_ASSERT(thread_data != nullptr, "Thread data is null");
      do {
        opt<message> msg = thread_data->rx_channel->await_message(std::chrono::milliseconds(200));
        if (!msg.has_value()) {
          continue;
        }

        handle_init_msg(*msg);
      } while (!checkpoints.initialized && !checkpoints.error_occurred && !thread_data->stoken.stop_requested());

      message ackmsg;
      ackmsg.category = ACKNOWLEDGEMENT;
      if (checkpoints.error_occurred) {
        ackmsg.type = MSGID_NACK;
        exit_code = -1;
      } else {
        ackmsg.type = MSGID_ACK;
      }
      thread_data->tx_channel->push(std::move(ackmsg));
    }

    void simulation_state::handle_init_msg(const message& msg) {
      if (msg.type == MSGID_THREAD_INITIALIZE) {
        checkpoints.initialized = true;

        /// launch necessary nodes
        sim_control->launch_nodes(config);
        std::println("simulation control launched\n");

        /// initialize all layers
        {
          std::lock_guard lock(state_mutex);
          /// load layers from config
          for (auto& layer : config.layers) {
            bind_layer(layer.path, layer.type);
          }
        }
      } else {
        std::print("Invalid message type for thread initialization : {}:{}\n", msg.category, msg.type);
        exit_code = -1;
        checkpoints.error_occurred = true;
      }
    }

    void simulation_state::bind_layer(const std::filesystem::path& path, simulation_layer_type type) {
      if (path.empty()) {
        std::print("Layer path is empty\n");
        return;
      }
      if (!std::filesystem::exists(path)) {
        std::print("Layer path does not exist : {}\n", path.string());
        return;
      }
      if (!std::filesystem::is_regular_file(path)) {
        std::print("Layer path is not a file : {}\n", path.string());
        return;
      }

      {
        lib_ptr lib = library_loader::load_library(path);
        if (!lib) {
          std::print("Failed to load library : {}\n", path.string());
          return;
        }
        lib->attempt_load();

        auto [itr, success] = layer_libraries.insert({ FNV(path.string()), std::move(lib) });
        if (!success) {
          std::print("Failed to insert library into map : {}\n", path.string());
          return;
        }
        if (!itr->second->is_loaded()) {
          std::println("Failed to load library : {}", path.string());
          return;
        }

        if (!itr->second->has_symbol("create_layer") || !itr->second->has_symbol("delete_layer")) {
          std::println("Failed to find creation/deletion symbols in layer library : 'create_layer'/'delete_layer'");
          itr->second->unload_library();
          layer_libraries.erase(itr);
          return;
        }

        const symbol& sym = itr->second->get_symbol("create_layer");
        const symbol& delete_sym = itr->second->get_symbol("delete_layer");
        if (!sym || !delete_sym) {
          std::print("Failed to find creation/deletion symbols in layer library : 'create_layer'/'delete_layer'\n");
          itr->second->unload_library();
          layer_libraries.erase(itr);
          return;
        }

        simulation_layer* layer_ptr = sym.get_as_callable<simulation_layer* (*)(simulation_config&)>()(config);
        if (layer_ptr == nullptr) {
          std::print("Failed to create layer from library : {}\n", path.string());
          itr->second->unload_library();
          layer_libraries.erase(itr);
          return;
        }

        sim_layer layer_data{
          layer_ptr,
          type,
          delete_sym,
        };
        auto [layer_itr, layer_success] = layer_map.insert({ FNV(path.string()), std::move(layer_data) });
        if (!layer_success) {
          std::print("Failed to insert layer into map : {}\n", path.string());
          delete_sym.get_as_callable<void (*)(simulation_layer*)>()(layer_ptr);
          itr->second->unload_library();
          layer_libraries.erase(itr);
          return;
        }

        layer_itr->second.layer->initialize();
      }
    }

    void simulation_state::wait_for_start() {
      TENSORLIB_ASSERT(thread_data != nullptr, "Thread data is null");
      do {
        opt<message> msg = thread_data->rx_channel->await_message(std::chrono::milliseconds(200));
        if (!msg) {
          continue;
        }
        handle_start_msg(*msg);
      } while (!checkpoints.running && !checkpoints.error_occurred && !thread_data->stoken.stop_requested());
    }

    void simulation_state::handle_start_msg(const message& msg) {
      if (msg.type == MSGID_THREAD_START) {
        checkpoints.running = true;

        /// publish start message to all nodes
      } else {
        std::print("Invalid message type for thread start : {}:{}\n", msg.category, msg.type);
        checkpoints.error_occurred = true;
        exit_code = -1;
      }
    }

    void simulation_state::wait_for_shutdown() {
      TENSORLIB_ASSERT(thread_data != nullptr, "Thread data is null");
      do {
        opt<message> msg = thread_data->rx_channel->await_message(std::chrono::milliseconds(200));
        if (!msg) {
          continue;
        }

        handle_shutdown_msg(*msg);
      } while (!checkpoints.running && !checkpoints.error_occurred && !thread_data->stoken.stop_requested());

      message ackmsg;
      ackmsg.category = ACKNOWLEDGEMENT;
      if (checkpoints.error_occurred) {
        ackmsg.type = MSGID_NACK;
        exit_code = -1;
      } else {
        ackmsg.type = MSGID_ACK;
      }
      thread_data->tx_channel->push(std::move(ackmsg));
    }

    void simulation_state::handle_shutdown_msg(const message& msg) {
      if (msg.type == MSGID_THREAD_SHUTDOWN) {
        checkpoints.running = false;
        checkpoints.finalized = true;

        std::print("Simulation thread shutting down\nclosing layers...");
        for (auto& layer : layer_map) {
          auto& layer_data = layer.second;
          layer_data.layer->shutdown();
          // if (layer_data.layer != nullptr) {
          //   layer_data.delete_fn.get_as_callable<void (*)(simulation_layer*)>()(layer_data.layer);
          //   layer_data.layer = nullptr;
          // }
        }
        std::print("offloading libraries...");
        for (auto& lib : layer_libraries) {
          lib.second = nullptr;
        }
        std::print("deallocation memory...");
        layer_libraries.clear();
        layer_map.clear();

        std::print("simulation thread shutdown complete\n");
      } else {
        std::print("Invalid message type for thread shutdown : {}:{}\n", msg.category, msg.type);
        checkpoints.error_occurred = true;
        exit_code = -1;
      }
    }

    void simulation_state::handle_simulation_event(const message& msg) {
      TENSORLIB_ASSERT(msg.category == SIMULATION_EVENT, "Invalid message category for simulation event");
      TENSORLIB_ASSERT(msg.type == MSGID_SIM_EVENT, "Invalid message type for simulation event");
      TENSORLIB_ASSERT(msg.data.size() > 0, "Simulation event message data is empty");

      auto event_msg = flexbuffers::GetRoot(msg.data).AsMap();
      auto id = event_msg["id"].AsInt64();
      auto time = event_msg["time"].AsInt64();
      auto node_ids = event_msg["node_ids"].AsVector();
      auto type = event_msg["type"].AsInt64();

      std::vector<natural_t> node_ids_vec;
      node_ids_vec.reserve(node_ids.size());
      for (natural_t i = 0; i < node_ids.size(); ++i) {
        node_ids_vec.push_back(node_ids[i].AsInt64());
      }
      simulation_event event(id, time, node_ids_vec, static_cast<event_type>(type));

      switch (event.type) {
        case SIM_EVENT_MAIN_STEP:
          do_main_timestep();
          break;

        case NODE_EVENT_INITIALIZE:
          break;

        case NODE_EVENT_SHUTDOWN:
          break;

        default: break;
      }
    }

  }  // namespace network
}  // namespace tensor