/**
 * \file core/serialization.hpp
 **/
#ifndef TENSORLIB_CORE_SERIALIZATION_HPP
#define TENSORLIB_CORE_SERIALIZATION_HPP

#include <concepts>
#include <span>
#include <sstream>
#include <string>

#include "core/types.hpp"

namespace tensor {
  namespace core {

    template <typename T>
    struct string_converter;

    template <typename T>
    concept has_string_writer = requires(const T& obj) {
      { T::write_string(obj) } -> std::same_as<std::string>;
    };

    template <typename T>
    concept trivially_string_writable = requires(const T& obj) {
      { std::to_string(obj) } -> std::same_as<std::string>;
    };

    template <typename T>
      requires(!container_type<T> && trivially_string_writable<T>)
    struct string_converter<T> {
      std::string operator()(const T& obj) const {
        return std::to_string(obj);
      }
    };

    template <typename T>
      requires(!container_type<T> && has_string_writer<T>)
    struct string_converter<T> {
      std::string operator()(const T& obj) const {
        return T::write_string(obj);
      }
    };

    template <typename T>
    concept has_string_converter = requires(const T& obj) {
      { string_converter<T>{}(obj) } -> std::same_as<std::string>;
    };

    template <typename CT>
      requires container_type<CT> && has_string_converter<typename CT::value_type>
    struct string_converter<CT> {
      std::string operator()(const CT& obj) const {
        std::stringstream ss;
        ss << "{\n(" << typeid(CT).name() << ")\n";
        if (obj.empty()) {
          ss << "  <empty>";
        } else {
          ss << "  ";
          for (natural_t i = 0; i < obj.size(); ++i) {
            ss << string_converter<typename CT::value_type>{}(obj[i]);
            if (i != obj.size() - 1) {
              ss << ", ";
            }
          }
        }

        ss << " \n}";
        return ss.str();
      }
    };

    template <typename T>
      requires has_string_converter<T>
    std::string as_string(const T& obj) {
      return string_converter<T>{}(obj);
    }

  }  // namespace core
}  // namespace tensor

#endif  // TENSORLIB_CORE_SERIALIZATION_HPP