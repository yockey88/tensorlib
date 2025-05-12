/**
 * \file random/mcmc.hpp
 **/
#ifndef TENSORLIB_RANDOM_MCMC_HPP
#define TENSORLIB_RANDOM_MCMC_HPP

#include "core/types.hpp"

#include "linalgebra/dyn_vector.hpp"
#include "random/rand.hpp"

namespace tensor {
  namespace random {

    template <typename Fn>
    concept prob_density_fn = requires(Fn fn, real_t x) {
      { fn(x) } -> std::same_as<real_t>;
    };

    template <prob_density_fn Fn>
    dyn_vector sample(Fn fn, natural_t num_samples) {
      dyn_vector samples = tensor::dyn_vector_rep(0.f, num_samples);
      for (natural_t i = 0; i < num_samples; ++i) {
        samples(i) = fn(random_generator::next_real());
      }
      return samples;
    }

    template <typename Fn>
    concept state_transition_fn = requires(Fn fn, real_t x) {
      { fn(x, std::declval<real_t>()) } -> std::same_as<std::pair<real_t, bool>>;
    };

    template <prob_density_fn Fn, state_transition_fn STFn>
    dyn_vector metropolis_hastings(Fn pdf_fn, STFn state_fn, real_t start, natural_t num_steps, real_t alpha = 1.f) {
      dyn_vector state_vector = tensor::dyn_vector_rep(0.f, num_steps);
      real_t current_state = start;

      for (natural_t i = 0; i < num_steps;) {
        auto [candidate, should_attempt_transition] = state_fn(current_state, alpha);
        if (!should_attempt_transition) {
          continue;
        }

        real_t prob = std::exp(std::log(pdf_fn(candidate)) - std::log(pdf_fn(current_state)));
        if (random_generator::next_real_0_1() < prob) {
          current_state = candidate;
        }

        state_vector(i) = current_state;

        ++i;
      }

      return state_vector;
    }

  }  // namespace random
}  // namespace tensor

#endif  // TENSORLIB_RANDOM_MCMC_HPP