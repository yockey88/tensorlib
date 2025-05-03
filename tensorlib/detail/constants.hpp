/**
 * @file detail/constants.hpp
 **/
#ifndef TENSORLIB_DETAIL_CONSTANTS_HPP
#define TENSORLIB_DETAIL_CONSTANTS_HPP

#include <limits>

#include "core/types.hpp"

namespace tensor {
  namespace detail {

    constexpr inline real_t epsilon = std::numeric_limits<real_t>::epsilon();
    constexpr inline real_t infinity = std::numeric_limits<real_t>::infinity();

    constexpr inline real_t pi = 3.14159265358979323846;
    constexpr inline real_t two_pi = 2.0 * pi;
    constexpr inline real_t pi_over_2 = pi / 2.0;

  }  // namespace detail
}  // namespace tensor

#endif  // TENSORLIB_DETAIL_CONSTANTS_HPP