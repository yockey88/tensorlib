/**
 * \file core/defines.hpp
 **/
#ifndef TENSORLIB_CORE_DEFINES_HPP
#define TENSORLIB_CORE_DEFINES_HPP

#ifdef TENSORLIB_REAL_TYPE_USE_DOUBLE
  #define TENSORLIB_REAL_USE_DOUBLE 1
#else
  #define TENSORLIB_REAL_USE_DOUBLE 0
#endif

#ifndef TENSORLIB_ASSERT
  #include <cassert>
  // clang-format off
  #define TENSORLIB_ASSERT(expr, msg) assert(expr && msg)
  // clang-format on
#endif

#endif  // TENSORLIB_CORE_DEFINES_HPP