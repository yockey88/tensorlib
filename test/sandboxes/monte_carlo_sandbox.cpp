/**
 * \file sandboxes/monte_carlo_sandbox.cpp
 **/
#include <print>

#include "tensorlib.hpp"

tensor::real_t random_walk_pdf(tensor::real_t x) {
  constexpr static tensor::vector<8> distribution{ 5.f, 10.f, 4.f, 4.f, 20.f, 20.f, 12.f, 5.f };

  tensor::real_t rand = tensor::random_generator::next_real();
  tensor::real_t sum = 0.f;
  for (tensor::natural_t i = 1; i <= 8; ++i) {
    sum += sum + distribution[i - 1];
    if (rand < sum) {
      return i;
    }
  }

  return tensor::epsilon;
}

std::pair<tensor::real_t, bool> random_walk_transition(tensor::real_t x, tensor::real_t alpha) {
  static std::vector<tensor::integer_t> options{ -1, 1 };
  tensor::real_t candidate = x + tensor::choose_from_n<tensor::integer_t>(options, 1)[0];
  if (candidate < 0 || candidate > 5) {
    return { 0, false };
  }
  return { candidate, true };
}

template <tensor::natural_t N>
tensor::dyn_vector perform_random_walk(tensor::real_t start, tensor::natural_t num_steps = 10'000) {
  tensor::dyn_vector out = tensor::random::metropolis_hastings(&random_walk_pdf, &random_walk_transition, start, num_steps);

  std::vector<tensor::natural_t> times_at_num;
  times_at_num.resize(6);
  for (tensor::natural_t i = 0; i < out.size; ++i) {
    TENSORLIB_ASSERT(out(i) >= 0 && out(i) <= 5, "Out of bounds");
    times_at_num[out(i)]++;
  }

  tensor::dyn_vector proportion = tensor::dyn_vector_rep(0.f, 6);
  for (tensor::natural_t i = 0; i < times_at_num.size(); ++i) {
    proportion(i) = static_cast<tensor::real_t>(times_at_num[i]) / static_cast<tensor::real_t>(out.size);
  }

  return proportion;
}

int main() {
  tensor::dyn_vector proportions = perform_random_walk<6>(4);
  std::println("proportions: {}", tensor::as_string(proportions));
  return 0;
}