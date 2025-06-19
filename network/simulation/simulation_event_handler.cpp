/**
 * \file network/simulation/simulation_event_handler.cpp
 **/
#include "simulation/simulation_event_handler.hpp"

#include <iostream>
#include <print>

#include "core/types.hpp"

#include "simulation/simulation_state.hpp"

namespace tensor {
  namespace network {

    bool scripted_event_timer::is_recurring() const {
      return key.interval > 0;
    }

    void scripted_event_timer::handle_timeout() {
      TENSORLIB_ASSERT(event_handler != nullptr, "Event handler is null");
      event_handler->trigger_event(key);
    }

    void simulation_event_handler::shutdown() {
      event_timers.clear();
      events.clear();
      sim_state = nullptr;
    }

    integer_t simulation_event_handler::register_scripted_event(const event_key& key, const simulation_event& event) {
      event_duration wait_period = event_duration(0);

      /// time until the step, the fire one-off event
      owning_ptr<scripted_event_timer> timer = nullptr;
      if (key.interval == 0 && key.time.min_step == key.time.max_step) {
        wait_period = event_duration(key.time.min_step * main_step_interval);
        timer = make_owning_ptr<scripted_event_timer>(this, io_context, wait_period, key);
      }
      /// handle types of repeating events
      else {
        if (key.time.min_step == key.time.max_step && key.time.max_step == 0) {
          wait_period = event_duration(key.interval);
        }
        /// repeating event starting at a certain time and lasting the rest of the sim
        else if (key.time.min_step == key.time.max_step && key.time.max_step > 0) {
          /// \todo calculate queue of wait times for timer
        }
        /// if not equal then we have a duration with the interval looping between start and end
        else if (key.time.min_step != key.time.max_step) {
          /// \todo calculate queue of wait times for timer
        } else {
          std::print(std::cerr, "Invalid event key: {}:{}\n", key.id, key.interval);
          return -1;
        }

        /// \todo create interval timer
      }
      TENSORLIB_ASSERT(timer != nullptr, "Timer is null");

      integer_t event_id = timer->get_event_id().value_or(simulation_event::generate_event_id());
      auto [emp_itr, res] = event_timers.emplace(key, event_handler(std::move(timer)));
      if (!res || emp_itr == event_timers.end()) {
        std::print(std::cerr, "Failed to register single event\n");
        return -1;
      }

      auto [emp_itr2, res2] = events.emplace(key, event);
      if (!res2 || emp_itr2 == events.end()) {
        std::print(std::cerr, "Failed to register event in event map\n");
        event_timers.erase(emp_itr);
        return -1;
      }

      if (started) {
        emp_itr->second.timer->start();
      }
      return event_id;
    }

    void simulation_event_handler::trigger_event(const event_key& key) {
      TENSORLIB_ASSERT(sim_state != nullptr, "Simulation state is null");

      auto itr = events.find(key);
      if (itr != events.end()) {
        /// trigger event
        sim_state->handle_event(itr->second);

        auto timer_itr = event_timers.find(key);
        if (timer_itr == event_timers.end() || timer_itr->second.timer == nullptr) {
          std::print(std::cerr, "Failed to find event timer for event {}:{}\n", key.id, key.interval);
          return;
        }

        if (!timer_itr->second.timer->is_recurring()) {
          /// remove timer from event_timers and erase the event from events
          /// we have to leave the timer alive so the io-context can call the cancel handler
          timer_itr->second.timer->stop();
          events.erase(itr);
        }
      } else {
        std::print(std::cerr, "Failed to find event {}:{}\n", key.id, key.interval);
      }
    }

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

  }  // namespace network
}  // namespace tensor
