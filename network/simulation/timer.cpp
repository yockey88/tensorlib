/**
 * \file simulation/timer.cpp
 **/
#include "simulation/timer.hpp"

#include <print>

namespace tensor {
  namespace network {

    asio::steady_timer& simulation_event_timer::Timer() {
      return timer;
    }

    void control_event_timer::start() {
      Timer().expires_after(interval);
      Timer().async_wait(std::bind_front(&control_event_timer::on_timeout, this));
    }

    void control_event_timer::stop() {
      Timer().cancel();
    }

    void control_event_timer::on_timeout(const asio::error_code& ec) {
      if (ec == asio::error::operation_aborted) {
        // timer cancelled
        return;
      }

      if (!ec) {
        callback->invoke();
        /// control event timer goes again until cancelled
        start();
      }
    }

  }  // namespace network
}  // namespace tensor