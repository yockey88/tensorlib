/**
 * \file linalgebra/dyn_vector.cpp
 **/
#include "linalgebra/dyn_vector.hpp"

#include <cmath>
#include <sstream>

#include "core/rand.hpp"

namespace tensor {

  dyn_vector::dyn_vector(natural_t s)
      : size(s) {
    TENSORLIB_ASSERT(size > 0, "Vector size must be greater than zero.");
    data.resize(size);
    for (size_t i = 0; i < size; ++i) {
      data[i] = 0;
    }
  }

  dyn_vector::dyn_vector(natural_t s, real_t vals)
      : size(s) {
    TENSORLIB_ASSERT(size > 0, "Vector size must be greater than zero.");
    data.resize(size);
    for (size_t i = 0; i < size; ++i) {
      data[i] = vals;
    }
  }

  dyn_vector::dyn_vector(dyn_vector&& other) noexcept
      : size(other.size), data(std::move(other.data)) {
    other.size = 0;
  }

  dyn_vector& dyn_vector::operator=(dyn_vector&& other) noexcept {
    if (this != &other) {
      size = other.size;
      data = std::move(other.data);
      other.size = 0;
    }
    return *this;
  }

  dyn_vector::dyn_vector(const dyn_vector& other) {
    data = other.data;
    size = other.size;
  }

  dyn_vector& dyn_vector::operator=(const dyn_vector& other) {
    if (this != &other) {
      data = other.data;
      size = other.size;
    }
    return *this;
  }

  std::string dyn_vector::write_string(const dyn_vector& v) {
    std::stringstream ss;
    ss << "\nVector [" << v.size << "]\n";
    for (natural_t i = 0; i < v.size; ++i) {
      ss << " " << v(i);
    }
    ss << "\n";
    return ss.str();
  }

  namespace detail {

    dyn_vector dyn_vector_sum_fn::operator()(const dyn_vector& v1, const dyn_vector& v2) const {
      if (v1.size != v2.size) {
        throw std::invalid_argument("Vector sizes must match for addition.");
      }

      dyn_vector res{ v1.size };
      for (natural_t i = 0; i < v1.size; ++i) {
        res(i) = detail::epsilon_sum(v1(i), v2(i));
      }
      return res;
    }

    dyn_vector dyn_vector_difference_fn::operator()(const dyn_vector& v1, const dyn_vector& v2) const {
      if (v1.size != v2.size) {
        throw std::invalid_argument("Vector sizes must match for subtraction.");
      }

      dyn_vector res{ v1.size };
      for (natural_t i = 0; i < v1.size; ++i) {
        res(i) = detail::epsilon_difference(v1(i), v2(i));
      }
      return res;
    }

    dyn_vector dyn_vector_hadamard_product_fn::operator()(const dyn_vector& v1, const dyn_vector& v2) const {
      if (v1.size != v2.size) {
        throw std::invalid_argument("Vector sizes must match for Hadamard product.");
      }

      dyn_vector res{ v1.size };
      for (natural_t i = 0; i < v1.size; ++i) {
        res(i) = detail::epsilon_product(v1(i), v2(i));
      }
      return res;
    }

    dyn_vector dyn_vector_scalar_product_fn::operator()(const dyn_vector& v, const real_t scalar) const {
      if (v.size == 0) {
        throw std::invalid_argument("Vector size must be greater than zero.");
      }

      dyn_vector res{ v.size };
      for (natural_t i = 0; i < v.size; ++i) {
        res(i) = detail::epsilon_product(v(i), scalar);
      }
      return res;
    }

    real_t dyn_vector_dot_product_fn::operator()(const dyn_vector& v1, const dyn_vector& v2) const {
      if (v1.size != v2.size) {
        throw std::invalid_argument("Vector sizes must match for dot product.");
      }

      real_t result = 0;
      for (natural_t i = 0; i < v1.size; ++i) {
        real_t prod = detail::epsilon_product(v1(i), v2(i));
        result = detail::epsilon_sum(result, prod);
      }
      return result;
    }

    real_t dyn_vector_magnitude_fn::operator()(const dyn_vector& v) const {
      real_t result = 0;
      for (natural_t i = 0; i < v.size; ++i) {
        result = detail::epsilon_sum(result, detail::epsilon_product(v(i), v(i)));
      }
      return std::sqrt(result);
    }

  }  // namespace detail

  dyn_vector rand_vector(natural_t size, real_t min, real_t max) {
    dyn_vector v{ size };
    for (natural_t i = 0; i < size; ++i) {
      v(i) = random::next_real() * (max - min) + min;
    }
    return v;
  }

}  // namespace tensor