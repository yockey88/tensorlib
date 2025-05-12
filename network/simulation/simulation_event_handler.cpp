/**
 * \file network/simulation/simulation_event_handler.cpp
 **/
#include "simulation/simulation_event_handler.hpp"

#include <iostream>
#include <print>

namespace tensor {
  namespace network {

    void simulation_event_handler::start_all() {
      for (auto& [key, timer] : event_timers) {
        TENSORLIB_ASSERT(timer.timer != nullptr, "Event timer is null");
        timer.timer->start();
      }
      started = true;
    }

    void simulation_event_handler::stop_all() {
      started = false;
      for (auto& [key, timer] : event_timers) {
        TENSORLIB_ASSERT(timer.timer != nullptr, "Event timer is null");
        timer.timer->stop();
      }
    }

    natural_t simulation_event_handler::generate_event_id() {
      static natural_t event_id = 1;  // main step used as 0
      return event_id++;
    }

  }  // namespace network
}  // namespace tensor
