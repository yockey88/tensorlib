/**
 * \file simulation/timer.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_TIMER_HPP
#define TENSORLIB_NETWORK_SIMULATION_TIMER_HPP

#include <chrono>
#include <iostream>

#include <asio/asio.hpp>

#include "core/owning_ptr.hpp"

#include "simulation/event_callback.hpp"

namespace tensor {
  namespace network {

    class simulation_event_timer {
     public:
      simulation_event_timer(asio::io_context& io_context, std::chrono::milliseconds interval)
          : interval(interval), timer(io_context) {}
      virtual ~simulation_event_timer() = default;

      void start();
      void stop();

      opt<integer_t> get_event_id() const;
      bool is_recurring() const;

     protected:
      asio::steady_timer& Timer();
      std::chrono::milliseconds interval;

      void set_event_id(integer_t id);
      void set_should_restart(bool flag);

     private:
      asio::steady_timer timer;

      bool should_restart = true;
      opt<integer_t> event_id = std::nullopt;

      void on_timeout(const asio::error_code& ec);
      virtual void handle_timeout() {}
    };

    class control_event_timer : public simulation_event_timer {
     public:
      control_event_timer(asio::io_context& io_context, std::chrono::milliseconds interval, owning_ptr<event_callback> callback)
          : simulation_event_timer(io_context, interval), callback(std::move(callback)) {}

      template <typename Fn, typename... Args>
      control_event_timer(asio::io_context& io_context, std::chrono::milliseconds interval, Fn&& on_timeout, Args&&... args)
          : simulation_event_timer(io_context, interval), callback(make_owning_ptr<event_invocable<Fn, Args...>>(std::forward<Fn>(on_timeout), std::forward<Args>(args)...)) {}

      virtual ~control_event_timer() = default;

     private:
      owning_ptr<event_callback> callback = nullptr;

      virtual void handle_timeout();
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_TIMER_HPP