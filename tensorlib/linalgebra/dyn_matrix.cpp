/**
 * \file linalgebra/dyn_matrix.cpp
 **/
#include "linalgebra/dyn_matrix.hpp"

#include <format>
#include <sstream>

#include "linalgebra/dyn_vector.hpp"
#include "random/rand.hpp"

namespace tensor {

  dyn_matrix::dyn_matrix(natural_t r, natural_t c)
      : rows(r), cols(c) {
    TENSORLIB_ASSERT(rows > 0 && cols > 0, "Matrix dimensions must be greater than zero.");
    data.resize(rows * cols);
    for (size_t i = 0; i < rows * cols; ++i) {
      data[i] = 0;
    }
  }

  dyn_matrix::dyn_matrix(natural_t r, natural_t c, real_t vals)
      : rows(r), cols(c) {
    TENSORLIB_ASSERT(rows > 0 && cols > 0, "Matrix dimensions must be greater than zero.");
    data.resize(rows * cols);
    for (size_t i = 0; i < rows * cols; ++i) {
      data[i] = vals;
    }
  }

  dyn_matrix::dyn_matrix(dyn_matrix&& other) noexcept
      : rows(other.rows), cols(other.cols), allocator(other.allocator), data(std::move(other.data)) {
    other.rows = 0;
    other.cols = 0;
  }

  dyn_matrix& dyn_matrix::operator=(dyn_matrix&& other) noexcept {
    if (this != &other) {
      rows = other.rows;
      cols = other.cols;
      data = std::move(other.data);
      other.rows = 0;
      other.cols = 0;
    }
    return *this;
  }

  dyn_matrix::dyn_matrix(const dyn_matrix& other) {
    data = other.data;
    rows = other.rows;
    cols = other.cols;
  }

  dyn_matrix& dyn_matrix::operator=(const dyn_matrix& other) {
    if (this != &other) {
      data = other.data;
      rows = other.rows;
      cols = other.cols;
    }
    return *this;
  }

  real_t& dyn_matrix::operator()(natural_t row, natural_t col) {
    TENSORLIB_ASSERT(row < rows && col < cols, "Index out of bounds.");
    return data[row * cols + col];
  }

  real_t dyn_matrix::operator()(natural_t row, natural_t col) const {
    TENSORLIB_ASSERT(row < rows && col < cols, "Index out of bounds.");
    return data[row * cols + col];
  }

  dyn_vector dyn_matrix::get_row(natural_t row) const {
    dyn_vector res{ cols };
    for (natural_t i = 0; i < cols; ++i) {
      res(i) = (*this)(row, i);
    }
    return res;
  }

  dyn_vector dyn_matrix::get_col(natural_t col) const {
    dyn_vector res{ rows };
    for (natural_t i = 0; i < rows; ++i) {
      res(i) = (*this)(i, col);
    }
    return res;
  }

  std::string dyn_matrix::write_string(const dyn_matrix& m) {
    std::stringstream ss;
    /// TODO: fix spacing

    ss << "\nMatrix [" << m.rows << " x " << m.cols << "]\n";
    for (natural_t i = 0; i < m.rows; ++i) {
      ss << "Row [" << i << "]: ";
      for (natural_t j = 0; j < m.cols; ++j) {
        ss << std::format("{:>.3f}", m(i, j));
        if (j != m.cols - 1) {
          ss << " | ";
        }
      }
      ss << "\n";
    }
    return ss.str();
  }

  namespace detail {

    dyn_matrix dyn_matrix_sum_fn::operator()(const dyn_matrix& m1, const dyn_matrix& m2) const {
      if (m1.rows != m2.rows || m1.cols != m2.cols) {
        throw std::invalid_argument("Matrix dimensions must match for addition.");
      }

      dyn_matrix res{ m1.rows, m1.cols };
      for (natural_t i = 0; i < m1.rows; ++i) {
        for (natural_t j = 0; j < m1.cols; ++j) {
          res(i, j) = detail::epsilon_sum(m1(i, j), m2(i, j));
        }
      }
      return res;
    }

    dyn_matrix dyn_matrix_difference_fn::operator()(const dyn_matrix& m1, const dyn_matrix& m2) const {
      if (m1.rows != m2.rows || m1.cols != m2.cols) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction.");
      }

      dyn_matrix res{ m1.rows, m1.cols };
      for (natural_t i = 0; i < m1.rows; ++i) {
        for (natural_t j = 0; j < m1.cols; ++j) {
          res(i, j) = detail::epsilon_difference(m1(i, j), m2(i, j));
        }
      }
      return res;
    }

    dyn_matrix dyn_matrix_product_fn::operator()(const dyn_matrix& m1, const dyn_matrix& m2) const {
      if (m1.cols != m2.rows) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
      }

      dyn_matrix res{ m1.rows, m1.cols };
      for (natural_t j = 0; j < m2.cols; ++j) {
        for (natural_t i = 0; i < m1.rows; ++i) {
          res(i, j) = dyn_vector_dot_product(m1.get_row(i), m2.get_col(j));
        }
      }

      return res;
    }

    dyn_matrix dyn_matrix_hadamard_product_fn::operator()(const dyn_matrix& m1, const dyn_matrix& m2) const {
      if (m1.rows != m2.rows || m1.cols != m2.cols) {
        throw std::invalid_argument("Matrix dimensions must match for Hadamard product.");
      }

      dyn_matrix res{ m1.rows, m1.cols };
      for (natural_t i = 0; i < m1.rows; ++i) {
        for (natural_t j = 0; j < m1.cols; ++j) {
          res(i, j) = detail::epsilon_product(m1(i, j), m2(i, j));
        }
      }

      return res;
    }

    dyn_vector dyn_matrix_vector_product_fn::operator()(const dyn_matrix& m, const dyn_vector& vector) const {
      if (m.cols != vector.size) {
        throw std::invalid_argument("Matrix and vector dimensions do not match for multiplication.");
      }

      dyn_vector result{ m.rows };
      for (natural_t i = 0; i < m.rows; ++i) {
        result(i) = dyn_vector_dot_product(m.get_row(i), vector);
      }
      return result;
    }

    dyn_vector dyn_matrix_vector_product_fn::operator()(const dyn_vector& v, const dyn_matrix& m) const {
      if (v.size != m.rows) {
        throw std::invalid_argument("Vector and matrix dimensions do not match for multiplication.");
      }

      dyn_vector result{ m.cols };
      for (natural_t j = 0; j < m.cols; ++j) {
        result(j) = dyn_vector_dot_product(v, m.get_col(j));
      }
      return result;
    }

    dyn_matrix dyn_matrix_diagonalize_vector_fn::operator()(const dyn_vector& vector) const {
      dyn_matrix res{ vector.size, vector.size };
      for (natural_t i = 0; i < vector.size; ++i) {
        res(i, i) = vector(i);
      }
      return res;
    }

  }  // namespace detail

  dyn_matrix rand_matrix(natural_t rows, natural_t cols, const real_t min, const real_t max) {
    dyn_matrix m{ rows, cols };
    for (natural_t i = 0; i < rows; ++i) {
      for (natural_t j = 0; j < cols; ++j) {
        m(i, j) = random_generator::next_real() * (max - min) + min;
      }
    }
    return m;
  }

}  // namespace tensor