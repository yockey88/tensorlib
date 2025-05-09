/**
 * \file core/types.hpp
 **/
#ifndef TENSORLIB_CORE_TYPES_HPP
#define TENSORLIB_CORE_TYPES_HPP

#include <cmath>
#include <cstdint>
#include <limits>

#include <gcem/gcem.hpp>

#include "core/defines.hpp"

namespace tensor {

  using index_t = uint64_t;

  using natural_t = uint64_t;
  using integer_t = int64_t;

  using real_t =
#if TENSORLIB_REAL_USE_DOUBLE
    double;
#else
    float;
#endif

  namespace detail {

    constexpr real_t abs(real_t x) {
      return x < 0 ? -x : x;
    }

    constexpr real_t default_epsilon() {
      return 1e-4;
    }

    constexpr bool epsilon_zero(real_t x, real_t epsilon = default_epsilon()) {
      return detail::abs(x) < epsilon;
    }

    constexpr bool epsilon_equal(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      return epsilon_zero(a - b, epsilon);
    }

    constexpr real_t epsilon_sum(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      real_t sum = a + b;
      return epsilon_zero(sum) ? 0 : sum;
    }

    constexpr real_t epsilon_difference(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      real_t diff = a - b;
      return epsilon_zero(diff) ? 0 : diff;
    }

    constexpr real_t epsilon_product(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      real_t product = a * b;
      return epsilon_zero(product) ? 0 : product;
    }

    constexpr real_t epsilon_division(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      if (epsilon_zero(b, epsilon)) {
        return 0;
      }
      return a / b;
    }

  }  // namespace detail
}  // namespace tensor

#endif  // TENSORLIB_CORE_TYPES_HPP