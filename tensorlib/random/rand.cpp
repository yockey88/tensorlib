/**
 * \file random/rand.cpp
 **/
#include "random/rand.hpp"

namespace tensor {

  rand_gen<natural_t> random_generator::gen_natural = rand_gen<natural_t>(0, UINT64_MAX);
  rand_gen<real_t> random_generator::gen_real = rand_gen<real_t>();
  rand_gen<real_t> random_generator::gen_real_0_1 = rand_gen<real_t>(0.0, 1.0);

  natural_t random_generator::next_natural() {
    return gen_natural.next();
  }

  real_t random_generator::next_real() {
    return gen_real.next();
  }

  real_t random_generator::next_real_0_1() {
    return gen_real_0_1.next();
  }

  void set_seed(uint32_t i1, uint32_t i2) {
    I1 = i1;
    I2 = i2;
  }

  void get_seed(uint32_t* i1, uint32_t* i2) {
    *i1 = I1;
    *i2 = I2;
  }

  real_t unif_rand() {
    I1 = 36969 * (I1 & 0177777) + (I1 >> 16);
    I2 = 18000 * (I2 & 0177777) + (I2 >> 16);
    return ((I1 << 16) ^ (I2 & 0177777)) * 2.328306437080797e-10; /* in [0,1) */
  }

}  // namespace tensor