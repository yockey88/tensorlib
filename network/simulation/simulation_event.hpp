/**
 * \file simulation/simulation_event.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_EVENT_HPP
#define TENSORLIB_NETWORK_SIMULATION_EVENT_HPP

#include <vector>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "message/message.hpp"
#include "simulation/timer.hpp"

namespace tensor {
  namespace network {

    enum event_type {
      SIM_EVENT_MAIN_STEP = 0,
      SIM_EVENT_REQUEST_STOP,
      SIM_EVENT_SHUTDOWN,

      NODE_EVENT_ACTIVATE,
      NODE_EVENT_REQUEST_SHUTDOWN,
    };

    struct event_time {
      natural_t min_step;
      natural_t max_step;

      event_time& operator=(const event_time& other);
    };

    struct event_key {
      integer_t id;
      natural_t interval;  /// or duration if time.min == time.max
      event_time time;

      constexpr auto operator<=>(const event_key& other) const {
        return id <=> other.id;
      }
    };

    // struct event_channel {
    //   asio::io_context& io_context;
    //   std::atomic<bool> raised = false;

    //   event_channel(asio::io_context& io_ctx) : io_context(io_ctx) {}

    //   void raise_event();
    //   void wait_for();
    // };

    struct simulation_event {
      natural_t id;
      event_time time;

      std::vector<natural_t> node_ids;
      event_type type;

      message get_message() const;

      static std::string write_string(const simulation_event& event);
      static integer_t generate_event_id() {
        static integer_t event_id = 0;
        return ++event_id;  /// pre-increment so as to not conflict with main step event
      }

      simulation_event() = default;
      simulation_event(natural_t id, natural_t time, const std::vector<natural_t>& node_ids, event_type type)
          : id(id), time(time), node_ids(node_ids), type(type) {}
    };

    static inline simulation_event sim_event(natural_t id, natural_t time, const std::vector<natural_t>& node_ids, event_type type) {
      return simulation_event(id, time, node_ids, type);
    }

    static inline simulation_event sim_event(natural_t id, natural_t time, event_type type) {
      return simulation_event(id, time, {}, type);
    }

    static inline simulation_event sim_event(event_type type) {
      return simulation_event(0, 0, {}, type);
    }

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_EVENT_HPP