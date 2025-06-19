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

    using event_duration = std::chrono::duration<natural_t, std::milli>;

    class simulation_event_handler;
    class simulation_state;

    class scripted_event_timer : public simulation_event_timer {
     public:
      scripted_event_timer(simulation_event_handler* event_handler, asio::io_context& io_context, event_duration interval, event_key key)
          : simulation_event_timer(io_context, interval), event_handler(event_handler), key(key) {
        set_event_id(key.id);
        set_should_restart(is_recurring());
      }
      virtual ~scripted_event_timer() = default;

     private:
      simulation_event_handler* event_handler;

      event_duration interval;
      event_key key;

      bool is_recurring() const;

      virtual void handle_timeout();
    };

    class simulation_event_handler : simulation_system {
     public:
      simulation_event_handler(asio::io_context& io_context)
          : io_context(io_context) {}
      ~simulation_event_handler() = default;

      void shutdown();

      inline void register_simulation_state(simulation_state* state) {
        sim_state = state;
      }

     protected:
      struct event_handler {
        owning_ptr<simulation_event_timer> timer;

        event_handler(owning_ptr<simulation_event_timer>&& timer)
            : timer(std::move(timer)) {}
      };

     public:
      integer_t register_scripted_event(const event_key& key, const simulation_event& event);
      void trigger_event(const event_key& key);

      void start_all();
      void stop_all();

      constexpr static inline integer_t kMainStepEventId = 0;
      constexpr static event_time kMainStepEventTime{ 0, 0 };
      constexpr static inline integer_t kMainStepEventDefaultInterval = 1000;
      constexpr static event_key kDefaultMainEventKey{ kMainStepEventId, kMainStepEventDefaultInterval, kMainStepEventTime };

      template <typename Fn, typename... Args>
      void set_main_step_interval(event_duration interval, Fn&& on_main_step, Args&&... args) {
        main_step_interval = interval;
        auto itr = event_timers.find(kDefaultMainEventKey);
        if (itr != event_timers.end()) {
          /// remove main step event
          event_timers.erase(itr);
        } else {
          /// no main step event before
        }

        owning_ptr<event_callback> callback = make_owning_ptr<event_invocable<Fn, Args...>>(std::forward<Fn>(on_main_step), std::forward<Args>(args)...);

        auto [emp_itr, res] = event_timers.emplace(
          event_key{ kMainStepEventId, interval.count(), kMainStepEventTime },
          event_handler(make_owning_ptr<control_event_timer>(io_context, interval, std::move(callback)))
        );

        if (!res || emp_itr == event_timers.end()) {
          std::print(std::cerr, "Failed to register main step event\n");
          return;
        }

        if (started) {
          emp_itr->second.timer->start();
        }
      }

     protected:
      bool started = false;
      asio::io_context& io_context;
      simulation_state* sim_state = nullptr;

      std::map<event_key, event_handler> event_timers;
      std::map<event_key, simulation_event> events;

      event_duration main_step_interval = event_duration(1000);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_EVENT_HANDLER_HPP