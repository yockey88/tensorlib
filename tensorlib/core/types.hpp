/**
 * \file core/types.hpp
 **/
#ifndef TENSORLIB_CORE_TYPES_HPP
#define TENSORLIB_CORE_TYPES_HPP

#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>

#include <gcem/gcem.hpp>

#include "core/defines.hpp"

namespace tensor {

  template <typename CT>
  concept has_size_method = requires(const CT& obj) {
    { obj.size() } -> std::same_as<size_t>;
  };

  template <typename CT>
  concept has_data_method =
    requires(const CT& obj) {
      { obj.data() } -> std::same_as<const typename CT::value_type*>;
    } ||
    requires(const CT& obj) {
      { obj.data() } -> std::same_as<typename CT::value_type*>;
    };

  template <typename CT>
  concept has_itr_methods = requires(const CT& obj) {
    { obj.begin() } -> std::same_as<typename CT::const_iterator>;
    { obj.end() } -> std::same_as<typename CT::const_iterator>;
  };

  template <typename CT>
  concept container_type = requires(const CT& obj) {
    /// is a container
    { obj.size() } -> std::same_as<size_t>;
    { obj.begin() } -> std::same_as<typename CT::const_iterator>;
    { obj.end() } -> std::same_as<typename CT::const_iterator>;
  };

  using index_t = uint64_t;

  using natural_t = uint64_t;
  using integer_t = int64_t;

  using real_t =
#if TENSORLIB_REAL_USE_DOUBLE
    double;
#else
    float;
#endif

  template <typename T>
  using opt = std::optional<T>;

  template <typename T>
  using cref = std::shared_ptr<T>;

  template <typename T, typename... Args>
    requires requires() {
      { std::make_shared<T>(std::declval<Args>()...) } -> std::same_as<cref<T>>;
    }
  cref<T> make_cref(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

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

    constexpr bool epsilon_lt(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      return epsilon_difference(a, b, epsilon) < 0;
    }

    constexpr bool epsilon_lte(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      return epsilon_difference(a, b, epsilon) <= 0;
    }

    constexpr bool epsilon_gt(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      return epsilon_difference(a, b, epsilon) > 0;
    }

    constexpr bool epsilon_gte(real_t a, real_t b, real_t epsilon = default_epsilon()) {
      return epsilon_difference(a, b, epsilon) >= 0;
    }

  }  // namespace detail
}  // namespace tensor

#endif  // TENSORLIB_CORE_TYPES_HPP