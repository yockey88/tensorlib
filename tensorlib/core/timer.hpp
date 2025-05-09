/**
 * \file core/timer.hpp
 **/
#ifndef TENSORLIB_CORE_TIMER_HPP
#define TENSORLIB_CORE_TIMER_HPP

#include <chrono>
#include <functional>
#include <tuple>

#include "core/types.hpp"

namespace tensor {
  namespace util {

    using clock = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;

    template <typename Rep>
    using duration = std::chrono::duration<real_t, Rep>;

    using ns_rep = std::chrono::nanoseconds;
    using ns = duration<ns_rep>;

    using us_rep = std::chrono::microseconds;
    using us = duration<us_rep>;

    using ms_rep = std::chrono::milliseconds;
    using ms = duration<ms_rep>;

    struct timer {
    };

    template <typename Fn, typename... Args>
    struct fn_timer {
      using ret_t = std::invoke_result_t<Fn, Args...>;

      Fn&& fn;
      std::tuple<Args...> args;

      fn_timer(Fn&& f, Args&&... a)
          : fn(std::forward<Fn>(f)), args(std::make_tuple(a...)) {}

      auto operator()() {
        return [this]<size_t... N>(std::index_sequence<N...>) -> std::pair<ret_t, real_t> {
          auto start = clock::now();

          ret_t result = std::invoke(std::forward<Fn>(fn), std::forward<Args>(std::get<N>(args))...);
          auto end = clock::now();

          return std::make_pair(std::move(result), std::chrono::duration_cast<us_rep>(end - start).count());
        }(std::make_index_sequence<sizeof...(Args)>{});
      }

      auto operator()(Fn&& f, Args&&... a) {
        fn = std::forward<Fn>(f);
        args = std::make_tuple(a...);
        return operator()();
      }
    };

    template <typename Fn, typename... Args>
    fn_timer(Fn&&, Args&&...) -> fn_timer<Fn, Args...>;

  }  // namespace util
}  // namespace tensor

#endif  // TENSORLIB_CORE_TIMER_HPP