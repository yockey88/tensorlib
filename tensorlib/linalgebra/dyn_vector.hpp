/**
 * \file linalgebra/dyn_vector.hpp
 **/
#ifndef TENSORLIB_LINALGEBRA_DYN_VECTOR_HPP
#define TENSORLIB_LINALGEBRA_DYN_VECTOR_HPP

#include <cstddef>

#include "core/buffer.hpp"
#include "core/ref.hpp"
#include "core/types.hpp"

#include "linalgebra/vector.hpp"

namespace tensor {

  struct dyn_vector : public ref_counted {
    natural_t size = 0;

    dyn_vector() = default;
    dyn_vector(natural_t s);
    dyn_vector(natural_t s, real_t vals);

    dyn_vector(dyn_vector&& other) noexcept;
    dyn_vector(const dyn_vector& other);
    dyn_vector& operator=(dyn_vector&& other) noexcept;
    dyn_vector& operator=(const dyn_vector& other);

    template <size_t N>
    dyn_vector(const std::array<real_t, N>& vals)
        : size(N) {
      TENSORLIB_ASSERT(size > 0, "Vector size must be greater than zero.");

      data.resize(size);
      for (size_t i = 0; i < N; ++i) {
        data[i] = vals[i];
      }
    }

    template <natural_t N>
    dyn_vector(const vector<N>& vals)
        : size(N) {
      TENSORLIB_ASSERT(size > 0, "Vector size must be greater than zero.");

      data.resize(size);
      for (natural_t i = 0; i < N; ++i) {
        data[i] = vals[i];
      }
    }

    real_t& operator[](natural_t i) { return data[i]; }
    real_t operator[](natural_t i) const { return data[i]; }

    real_t& operator()(natural_t i) { return data[i]; }
    real_t operator()(natural_t i) const { return data[i]; }

    static std::string write_string(const dyn_vector& v);

    std::vector<real_t> data;
  };

  using vectorx = ref<dyn_vector>;

  static inline vectorx make_vector(natural_t size) {
    return vectorx::create(size);
  }

  namespace detail {

    struct dyn_vector_sum_fn {
      dyn_vector operator()(const dyn_vector& v1, const dyn_vector& v2) const;
    };

    struct dyn_vector_difference_fn {
      dyn_vector operator()(const dyn_vector& v1, const dyn_vector& v2) const;
    };

    struct dyn_vector_hadamard_product_fn {
      dyn_vector operator()(const dyn_vector& v1, const dyn_vector& v2) const;
    };

    struct dyn_vector_scalar_product_fn {
      dyn_vector operator()(const dyn_vector& v, const real_t scalar) const;
    };

    struct dyn_vector_dot_product_fn {
      real_t operator()(const dyn_vector& v1, const dyn_vector& v2) const;
    };

    struct dyn_vector_magnitude_fn {
      real_t operator()(const dyn_vector& v) const;
    };

  }  // namespace detail

  constexpr inline detail::dyn_vector_sum_fn dyn_vector_sum{};
  constexpr inline detail::dyn_vector_difference_fn dyn_vector_difference{};
  constexpr inline detail::dyn_vector_hadamard_product_fn dyn_vector_hadamard_product{};
  constexpr inline detail::dyn_vector_scalar_product_fn dyn_vector_scalar_product{};
  constexpr inline detail::dyn_vector_dot_product_fn dyn_vector_dot_product{};
  constexpr inline detail::dyn_vector_magnitude_fn dyn_vector_magnitude{};

  dyn_vector rand_vector(natural_t size, real_t min = -1.f, real_t max = 1.f);

}  // namespace tensor

#endif  // TENSORLIB_LINALGEBRA_DYN_VECTOR_HPP