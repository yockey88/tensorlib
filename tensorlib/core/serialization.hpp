/**
 * \file core/serialization.hpp
 **/
#ifndef TENSORLIB_CORE_SERIALIZATION_HPP
#define TENSORLIB_CORE_SERIALIZATION_HPP

#include <concepts>
#include <string>

namespace tensor {
  namespace core {

    template <typename T>
      requires requires(const T& obj) {
        { T::write_string(obj) } -> std::same_as<std::string>;
      }
    std::string as_string(const T& obj) {
      return T::write_string(obj);
    }

  }  // namespace core
}  // namespace tensor

#endif  // TENSORLIB_CORE_SERIALIZATION_HPP