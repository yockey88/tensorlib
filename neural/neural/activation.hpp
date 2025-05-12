/**
 * \file neural/activation.hpp
 **/
#ifndef TENSORLIB_NEURAL_ACTIVATION_HPP
#define TENSORLIB_NEURAL_ACTIVATION_HPP

#include <utility>

#include "core/types.hpp"

#include "linalgebra/dyn_vector.hpp"
#include "linalgebra/vector.hpp"

namespace tensor {
  namespace neural {
    namespace detail {

      // constexpr bool isnan(real_t x) {
      //   return !(x == x);
      // }

      // Sigmoid activation function
      constexpr real_t sigmoid(real_t x) {
        return 1.0f / (1.0f + gcem::exp(-x));
      }

      constexpr real_t sigmoid_derivative(real_t x) {
        return sigmoid(x) * (1.0f - sigmoid(x));
      }

      constexpr real_t relu(real_t x) {
        return x > 0 ? x : 0;
      }

      constexpr real_t relu_derivative(real_t x) {
        return x > 0 ? 1.f : 0.f;
      }

    }  // namespace detail

    template <natural_t N, typename Fn, size_t... I>
      requires std::is_invocable_r_v<real_t, Fn, real_t>
    constexpr std::array<real_t, N> activate_vector_element(const vector<N>& v, Fn&& fn, std::index_sequence<I...>) {
      return std::array<real_t, N>{ std::invoke(std::forward<Fn>(fn), v[I])... };
    }

    template <natural_t N>
    constexpr vector<N> sigmoid(const vector<N>& v) {
      return vector<N>{ activate_vector_element(v, &detail::sigmoid, std::make_index_sequence<N>{}) };
    }

    template <natural_t N>
    constexpr vector<N> sigmoid_derivative(const vector<N>& v) {
      return vector<N>{ activate_vector_element(v, [](real_t x) { return detail::sigmoid(x) * (1 - detail::sigmoid(x)); }, std::make_index_sequence<N>{}) };
    }

    template <natural_t N>
    constexpr vector<N> relu(const vector<N>& v) {
      return vector<N>{ activate_vector_element(v, &detail::relu, std::make_index_sequence<N>{}) };
    }

    template <natural_t N>
    constexpr vector<N> relu_derivative(const vector<N>& v) {
      return vector<N>{ activate_vector_element(v, [](real_t x) { return x > 0 ? 1.f : 0.f; }, std::make_index_sequence<N>{}) };
    }

    // template <natural_t N>
    // constexpr vector<N> clipping(const vector<N>& v, real_t min, real_t max) {
    //   return vector<N>{ activate_vector_element(v, [min, max](real_t x) { return std::clamp(x, min, max); }, std::make_index_sequence<N>{}) };
    // }

    // template <natural_t N>
    // constexpr vector<N> activate(const vector<N>& v) {
    //   return vector<N>{ activate_vector_element(v, [](real_t x) -> real_t { return x <= 0.f ? -1.f : 1.f; }, std::make_index_sequence<N>{}) };
    // }

    dyn_vector sigmoid(const dyn_vector& v);
    dyn_vector sigmoid_derivative(const dyn_vector& v);

    dyn_vector relu(const dyn_vector& v);
    dyn_vector relu_derivative(const dyn_vector& v);

    // dyn_vector activate(dyn_vector& v);

    using derivative_fn_t = dyn_vector (*)(const dyn_vector&);
    using dyn_activation_fn_t = dyn_vector (*)(const dyn_vector&);

    struct layer_activation {
      dyn_activation_fn_t activation_function = nullptr;
      derivative_fn_t derivative_function = nullptr;
    };

    constexpr inline layer_activation sigmoid_layer = { &sigmoid, &sigmoid_derivative };
    constexpr inline layer_activation relu_layer = { &relu, &relu_derivative };

  }  // namespace neural
}  // namespace tensor

#endif  // TENSORLIB_NEURAL_ACTIVATION_HPP