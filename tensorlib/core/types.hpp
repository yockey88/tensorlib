/**
 * \file core/types.hpp
 **/
#ifndef TENSORLIB_CORE_TYPES_HPP
#define TENSORLIB_CORE_TYPES_HPP

#include <cstdint>

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

}  // namespace tensor

#endif  // TENSORLIB_CORE_TYPES_HPP