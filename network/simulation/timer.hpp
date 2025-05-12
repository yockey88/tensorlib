/**
 * \file simulation/timer.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_TIMER_HPP
#define TENSORLIB_NETWORK_SIMULATION_TIMER_HPP

#include <chrono>
#include <iostream>

#include <asio/asio.hpp>

#include "simulation/event_callback.hpp"

namespace tensor {
  namespace network {

    class simulation_event_timer {
     public:
      simulation_event_timer(asio::io_context& io_context)
          : timer(io_context) {}
      virtual ~simulation_event_timer() = default;

      virtual void start() = 0;
      virtual void stop() = 0;

     protected:
      asio::steady_timer& Timer();

     private:
      asio::steady_timer timer;
    };

    class control_event_timer : public simulation_event_timer {
     public:
      control_event_timer(asio::io_context& io_context, std::chrono::milliseconds interval, owning_ptr<event_callback> callback)
          : simulation_event_timer(io_context), callback(std::move(callback)), interval(interval) {}
      virtual ~control_event_timer() = default;

      void start() override;
      void stop() override;

     private:
      owning_ptr<event_callback> callback = nullptr;
      std::chrono::milliseconds interval;

      void on_timeout(const asio::error_code& ec);
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_TIMER_HPP