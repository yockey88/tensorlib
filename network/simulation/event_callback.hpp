/**
 * \file simulation/event_callback.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_EVENT_CALLBACK_HPP
#define TENSORLIB_NETWORK_SIMULATION_EVENT_CALLBACK_HPP

#include <chrono>
#include <tuple>

#include "core/types.hpp"

namespace tensor {
  namespace network {

    struct event_callback {
      virtual ~event_callback() = default;
      void invoke() {
        this->operator()();
      }
      virtual void operator()() = 0;
    };

    template <typename Fn, typename... Args>
      requires requires(Fn fn, Args... args) {
        { fn(args...) } -> std::same_as<void>;
      }
    class event_invocable : public event_callback {
     public:
      event_invocable(Fn fn, Args... args)
          : fn(std::move(fn)), args(std::move(args)...) {}
      ~event_invocable() override = default;

      void operator()() override {
        return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
      }

     private:
      Fn fn;
      std::tuple<Args...> args;
    };
    template <typename Fn, typename... Args>
    event_invocable(Fn fn, Args... args) -> event_invocable<Fn, Args...>;

    struct event_key {
      natural_t id;
      std::chrono::milliseconds interval;

      constexpr auto operator<=>(const event_key& other) const {
        return id <=> other.id;
      }
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_EVENT_CALLBACK_HPP