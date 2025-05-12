/**
 * \file simulation/simulation_event_handler.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_EVENT_HANDLER_HPP
#define TENSORLIB_NETWORK_SIMULATION_EVENT_HANDLER_HPP

#include <chrono>
#include <iostream>
#include <map>

#include <asio/asio.hpp>

#include "core/types.hpp"

#include "simulation/event_callback.hpp"
#include "simulation/simulation_event.hpp"
#include "simulation/system.hpp"
#include "simulation/timer.hpp"


namespace tensor {
  namespace network {

    class simulation_event_handler : simulation_system {
     public:
      simulation_event_handler(asio::io_context& io_context)
          : io_context(io_context) {}
      ~simulation_event_handler() = default;

      template <typename Fn, typename... Args>
      void set_main_step_interval(std::chrono::milliseconds interval, Fn&& on_main_step, Args&&... args) {
        main_step_interval = interval;
        auto itr = event_timers.find(event_key{ 0 });
        if (itr != event_timers.end()) {
          /// remove main step event
          event_timers.erase(itr);
        } else {
          /// no main step event before
        }

        owning_ptr<event_callback> callback = make_owning_ptr<event_invocable<Fn, Args...>>(std::forward<Fn>(on_main_step), std::forward<Args>(args)...);

        auto [emp_itr, res] =
          event_timers.emplace(
            event_key{ 0, interval },
            event_handler{
              .timer = make_owning_ptr<control_event_timer>(io_context, interval, std::move(callback)),
            }
          );

        if (!res || emp_itr == event_timers.end()) {
          std::print(std::cerr, "Failed to register main step event\n");
          return;
        }

        if (started) {
          emp_itr->second.timer->start();
        }
      }

      void start_all();
      void stop_all();

     protected:
      bool started = false;
      asio::io_context& io_context;

      struct event_handler {
        owning_ptr<simulation_event_timer> timer;
      };

      std::map<event_key, event_handler> event_timers;

      std::chrono::milliseconds main_step_interval = std::chrono::milliseconds(1000);

      natural_t generate_event_id();
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_EVENT_HANDLER_HPP