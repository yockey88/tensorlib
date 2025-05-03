/**
 * \file core/activation_functions.hpp
 **/
#ifndef TENSORLIB_CORE_ACTIVATION_FUNCTIONS_HPP
#define TENSORLIB_CORE_ACTIVATION_FUNCTIONS_HPP

#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>
#include <numeric>

#include "core/types.hpp"

namespace tensor {
  namespace activation {
    namespace detail {

      // constexpr bool isnan(real_t x) {
      //   return !(x == x);
      // }

      constexpr real_t exp(real_t x) {
        if (x == 0) {
          return 1.0f;
        }

        if (x > 0) {
          return detail::exp(x - 1) * std::numbers::e_v<real_t>;
        } else {
          return 1.0 / detail::exp(-x);
        }
      }

    }  // namespace detail

    // Activation function interface
    template <typename T>
    struct ActivationFunction {
      virtual T operator()(T x) const = 0;
      virtual ~ActivationFunction() = default;
    };

    // Sigmoid activation function
    struct Sigmoid : public ActivationFunction<float> {
      constexpr float operator()(float x) const override {
        return 1.0f / (1.0f + detail::exp(-x));
      }
    };

    // ReLU activation function
    struct ReLU : public ActivationFunction<float> {
      constexpr float operator()(float x) const override {
        return x > 0 ? x : 0;
      }
    };

  }  // namespace activation
}  // namespace tensor

#endif  // TENSORLIB_CORE_ACTIVATION_FUNCTIONS_HPP