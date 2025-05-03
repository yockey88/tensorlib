/**
 * \file core/types.hpp
 **/
#ifndef TENSORLIB_CORE_TYPES_HPP
#define TENSORLIB_CORE_TYPES_HPP

#include <cstdint>
#include <limits>

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

  }  // namespace detail

  constexpr bool epsilon_difference(real_t a, real_t b, real_t epsilon = std::numeric_limits<real_t>::epsilon()) {
    return detail::abs(a - b) > epsilon;
  }

  constexpr bool epsilon_equal(real_t a, real_t b, real_t epsilon = std::numeric_limits<real_t>::epsilon()) {
    return detail::abs(a - b) < epsilon;
  }

}  // namespace tensor

#endif  // TENSORLIB_CORE_TYPES_HPP