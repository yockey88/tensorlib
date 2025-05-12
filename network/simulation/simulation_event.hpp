/**
 * \file simulation/simulation_event.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_EVENT_HPP
#define TENSORLIB_NETWORK_SIMULATION_EVENT_HPP

#include <vector>

#include "core/types.hpp"

#include "message/message.hpp"

namespace tensor {
  namespace network {

    enum event_type {
      SIM_EVENT_MAIN_STEP = 0,
      SIM_EVENT_REQUEST_STOP,
      SIM_EVENT_SHUTDOWN,

      NODE_EVENT_INITIALIZE,
      NODE_EVENT_SHUTDOWN,
    };

    struct simulation_event {
      natural_t id;
      natural_t time;

      std::vector<natural_t> node_ids;
      event_type type;

      message get_message() const;

      static std::string write_string(const simulation_event& event);

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