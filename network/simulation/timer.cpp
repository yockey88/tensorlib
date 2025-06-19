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

    void simulation_event_timer::start() {
      Timer().expires_after(interval);
      Timer().async_wait(std::bind_front(&simulation_event_timer::on_timeout, this));
    }

    void simulation_event_timer::stop() {
      Timer().cancel();
    }

    opt<integer_t> simulation_event_timer::get_event_id() const {
      return event_id;
    }

    bool simulation_event_timer::is_recurring() const {
      return should_restart;
    }

    void simulation_event_timer::set_event_id(integer_t id) {
      event_id = id;
    }

    void simulation_event_timer::set_should_restart(bool flag) {
      should_restart = flag;
    }

    void simulation_event_timer::on_timeout(const asio::error_code& ec) {
      if (ec == asio::error::operation_aborted) {
        // timer cancelled
        return;
      }

      if (!ec) {
        handle_timeout();
        /// control event timer goes again until cancelled
        if (should_restart) {
          start();
        }
      }
    }

    void control_event_timer::handle_timeout() {
      if (callback != nullptr) {
        callback->invoke();
      } else {
        std::print(std::cerr, "Control event timer callback is null\n");
      }
    }

  }  // namespace network
}  // namespace tensor