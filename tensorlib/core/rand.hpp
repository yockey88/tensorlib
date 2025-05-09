/**
 * \file core/rand.hpp
 **/
#ifndef TENSORLIB_CORE_RAND_HPP
#define TENSORLIB_CORE_RAND_HPP

#include <random>

#include "core/types.hpp"

namespace tensor {

  template <typename T>
  class rand_gen {
   public:
    rand_gen() : gen(rand()) {}
    rand_gen(T min, T max) : gen(rand()), dist(min, max) {}

    T Next() { return dist(gen); }

   private:
    std::random_device rand;
    std::mt19937 gen;
    std::uniform_int_distribution<T> dist;
  };

  template <>
  class rand_gen<real_t> {
   public:
    rand_gen()
        : gen(rand()) {}
    rand_gen(real_t min, real_t max)
        : gen(rand()), dist(min, max) {}

    real_t Next() { return dist(gen); }

   private:
    std::random_device rand;
    std::mt19937 gen;
    std::uniform_real_distribution<real_t> dist;
  };

  class random {
   public:
    static natural_t next_natural();
    static real_t next_real();
    static real_t next_real_0_1();

   private:
    static rand_gen<natural_t> gen_natural;
    static rand_gen<real_t> gen_real;
    static rand_gen<real_t> gen_real_0_1;
  };

}  // namespace tensor

#endif  // TENSORLIB_CORE_RAND_HPP