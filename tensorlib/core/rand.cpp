/**
 * \file core/rand.cpp
 **/
#include "core/rand.hpp"

namespace tensor {

  rand_gen<natural_t> random::gen_natural = rand_gen<natural_t>(0, UINT64_MAX);
  rand_gen<real_t> random::gen_real = rand_gen<real_t>();
  rand_gen<real_t> random::gen_real_0_1 = rand_gen<real_t>(0.0, 1.0);

  natural_t random::next_natural() {
    return gen_natural.Next();
  }

  real_t random::next_real() {
    return gen_real.Next();
  }

  real_t random::next_real_0_1() {
    return gen_real_0_1.Next();
  }

}  // namespace tensor