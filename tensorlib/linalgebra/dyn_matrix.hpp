/**
 * \file linalgebgra/dyn_matrix.hpp
 **/
#ifndef TENSORLIB_LINALGEBRA_DYN_MATRIX_HPP
#define TENSORLIB_LINALGEBRA_DYN_MATRIX_HPP

#include <cstddef>

// #include "core/buffer.hpp"
#include "core/ref.hpp"
#include "core/types.hpp"

#include "linalgebra/dyn_vector.hpp"
#include "linalgebra/matrix.hpp"

namespace tensor {

  struct dyn_matrix : public ref_counted {
    natural_t rows = 0;
    natural_t cols = 0;

    dyn_matrix() = default;
    dyn_matrix(natural_t r, natural_t c);
    dyn_matrix(natural_t r, natural_t c, real_t vals);

    dyn_matrix(dyn_matrix&& other) noexcept;
    dyn_matrix(const dyn_matrix& other);
    dyn_matrix& operator=(dyn_matrix&& other) noexcept;
    dyn_matrix& operator=(const dyn_matrix& other);

    template <size_t R, size_t C, size_t N>
    dyn_matrix(const std::array<real_t, N>& vals)
        : rows(R), cols(C) {
      static_assert(N == R * C, "Array size must match matrix dimensions.");
      TENSORLIB_ASSERT(rows > 0 && cols > 0, "Matrix dimensions must be greater than zero.");

      data.resize(rows * cols);
      for (size_t i = 0; i < N; ++i) {
        data[i] = vals[i];
      }
    }

    template <size_t R, size_t C>
    dyn_matrix(const std::array<std::array<real_t, C>, R>& vals)
        : rows(R), cols(C) {
      TENSORLIB_ASSERT(rows > 0 && cols > 0, "Matrix dimensions must be greater than zero.");
      data.resize(rows * cols);
      for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
          (*this)(i, j) = vals[i][j];
        }
      }
    }

    template <natural_t R, natural_t C>
    dyn_matrix(const matrix<R, C>& vals)
        : rows(R), cols(C) {
      TENSORLIB_ASSERT(rows > 0 && cols > 0, "Matrix dimensions must be greater than zero.");
      data.resize(rows * cols);
      for (natural_t i = 0; i < R; ++i) {
        for (natural_t j = 0; j < C; ++j) {
          (*this)(i, j) = vals(i, j);
        }
      }
    }

    real_t& operator()(natural_t row, natural_t col);
    real_t operator()(natural_t row, natural_t col) const;

    real_t& operator[](natural_t i) { return data[i]; }
    real_t operator[](natural_t i) const { return data[i]; }

    dyn_vector get_row(natural_t row) const;
    dyn_vector get_col(natural_t col) const;

    static std::string write_string(const dyn_matrix& m);
    // static std::vector<uint8_t> write_binary(ref<dyn_matrix>& m);

    memory::arena_allocator<real_t> allocator{};

    std::vector<real_t> data;
  };

  using matrix_nxm = ref<dyn_matrix>;

  static inline matrix_nxm make_matrix(natural_t rows, natural_t cols) {
    return matrix_nxm::create(rows, cols);
  }

  template <natural_t R, natural_t C>
  static inline matrix_nxm make_matrix(const matrix<R, C>& vals) {
    return matrix_nxm::create(vals);
  }

  namespace detail {

    struct dyn_matrix_sum_fn {
      dyn_matrix operator()(const dyn_matrix& m1, const dyn_matrix& m2) const;
    };

    struct dyn_matrix_difference_fn {
      dyn_matrix operator()(const dyn_matrix& m1, const dyn_matrix& m2) const;
    };

    struct dyn_matrix_product_fn {
      dyn_matrix operator()(const dyn_matrix& m1, const dyn_matrix& m2) const;
    };

    struct dyn_matrix_hadamard_product_fn {
      dyn_matrix operator()(const dyn_matrix& m1, const dyn_matrix& m2) const;
    };

    struct dyn_matrix_vector_product_fn {
      dyn_vector operator()(const dyn_matrix& m, const dyn_vector& v) const;
    };

  }  // namespace detail

  constexpr inline detail::dyn_matrix_sum_fn dyn_matrix_sum{};
  constexpr inline detail::dyn_matrix_difference_fn dyn_matrix_difference{};
  constexpr inline detail::dyn_matrix_product_fn dyn_matrix_product{};
  constexpr inline detail::dyn_matrix_hadamard_product_fn dyn_matrix_hadamard_product{};
  constexpr inline detail::dyn_matrix_vector_product_fn dyn_matrix_vector_product{};

  dyn_matrix rand_matrix(natural_t rows, natural_t cols, real_t min = -1.f, real_t max = 1.f);

}  // namespace tensor

#endif  // TENSORLIB_LINALGEBRA_DYN_MATRIX_HPP