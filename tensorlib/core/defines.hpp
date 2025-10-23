/**
 * \file core/defines.hpp
 **/
#ifndef TENSORLIB_CORE_DEFINES_HPP
#define TENSORLIB_CORE_DEFINES_HPP

#include <cstdint>
#include <string_view>

/// dynamic library macros for layers and plugins

#ifdef TENSORLIB_WINDOWS
  #ifdef TENSORLIB_MODULE
    #define TENSORLIB_API extern "C" __declspec(dllexport)
    #define TENSORLIB_CLASS __declspec(dllexport)
  #else
    #define TENSORLIB_API extern "C" __declspec(dllimport)
    #define TENSORLIB_CLASS __declspec(dllimport)
  #endif
#endif  // TENSORLIB_WINDOWS

#ifdef TENSORLIB_LINUX
  #ifdef TENSORLIB_MODULE
    #define TENSORLIB_API __attribute__((visibility("default")))
  #endif
#endif  // TENSORLIB_LINUX

#ifdef TENSORLIB_MAC
  #ifdef TENSORLIB_MODULE
    #define TENSORLIB_API __attribute__((visibility("default")))
  #endif
#endif  // TENSORLIB_MAC

#ifndef TENSORLIB_API
  #define TENSORLIB_API
#endif
#ifndef TENSORLIB_CLASS
  #define TENSORLIB_CLASS
#endif
#define TENSORAPI TENSORLIB_API
#define TENSORCLASS TENSORLIB_CLASS

#ifdef TENSORLIB_REAL_TYPE_USE_DOUBLE
  #define TENSORLIB_REAL_USE_DOUBLE 1
#else
  #define TENSORLIB_REAL_USE_DOUBLE 0
#endif

#define BINDFN(fn) std::bind_front(fn, this)

static constexpr uint64_t kFnvOffsetBasis = 0xBCF29CE484222325;
static constexpr uint64_t kFnvPrime = 0x100000001B3;

constexpr uint64_t FNV(std::string_view str) {
  uint64_t hash = kFnvOffsetBasis;
  for (auto& c : str) {
    hash ^= c;
    hash *= kFnvPrime;
  }
  hash ^= str.length();
  hash *= kFnvPrime;

  return hash;
}

#define bit(x) (1 << (x))

#endif  // TENSORLIB_CORE_DEFINES_HPP