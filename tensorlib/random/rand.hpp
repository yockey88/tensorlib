/**
 * \file random/rand.hpp
 **/
#ifndef TENSORLIB_CORE_RAND_HPP
#define TENSORLIB_CORE_RAND_HPP

#include <cstdint>
#include <random>
#include <span>
#include <vector>

#include "core/types.hpp"
#include "detail/constants.hpp"


namespace tensor {

  struct probability {
    constexpr probability() : value(epsilon) {}
    constexpr probability(real_t val) : value(val) {
      if (detail::epsilon_lte(val, 0.f)) {
        value = epsilon;
      }
      if (detail::epsilon_gt(val, 1.f)) {
        value = 1.f;
      }
    }

    operator real_t() const { return value; }

    constexpr auto operator<=>(const probability&) const = default;
    constexpr auto operator<=>(const real_t& other) const {
      return value <=> other;
    }

   private:
    real_t value;
  };

  template <typename T>
  class rand_gen {
   public:
    rand_gen() : gen(rand()) {}
    rand_gen(T min, T max) : gen(rand()), dist(min, max) {}

    T next() { return dist(gen); }

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
        : gen(rand()) {
      if (min > max) {
        std::swap(min, max);
      }
      if (min == max) {
        min = 0;
        max = 1;
      }
      dist = std::uniform_real_distribution<real_t>(min, max);
    }

    real_t next() { return dist(gen); }

   private:
    std::random_device rand;
    std::mt19937 gen;
    std::uniform_real_distribution<real_t> dist;
  };

  class random_generator {
   public:
    static natural_t next_natural();
    static real_t next_real();
    static real_t next_real_0_1();

   private:
    static rand_gen<natural_t> gen_natural;
    static rand_gen<real_t> gen_real;
    static rand_gen<real_t> gen_real_0_1;
  };

  static uint32_t I1 = 1234, I2 = 5678;

  template <typename T>
  std::vector<T> choose_from_n(const std::span<const T> options, natural_t num_samples) {
    std::vector<T> samples;

    auto gen = std::mt19937{ std::random_device{}() };
    std::ranges::sample(options, std::back_inserter(samples), 1, gen);

    TENSORLIB_ASSERT(samples.size() == num_samples, "Invalid number of samples");
    return samples;
  }

  void set_seed(uint32_t i1, uint32_t i2);
  void get_seed(uint32_t* i1, uint32_t* i2);
  real_t unif_rand();

}  // namespace tensor

#endif  // TENSORLIB_CORE_RAND_HPP