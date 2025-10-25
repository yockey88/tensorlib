/**
 * \file tensorlib.hpp
 **/
#ifndef TENSORLIB_HPP
#define TENSORLIB_HPP

#include "detail/constants.hpp"
#include "detail/tensorlib_state.hpp"

#include "core/arena.hpp"
#include "core/arena_allocator.hpp"
#include "core/buffer.hpp"
#include "core/defines.hpp"
#include "core/serialization.hpp"
#include "core/timer.hpp"
#include "core/types.hpp"
#include "math/n_choose_k.hpp"

#include "graph/graph.hpp"
#include "linalgebra/dyn_matrix.hpp"
#include "linalgebra/dyn_vector.hpp"
#include "linalgebra/matrix.hpp"
#include "linalgebra/vector.hpp"
#include "random/mcmc.hpp"
#include "random/rand.hpp"


namespace tensor {

  /// user friendly types and wrappers

  using vec1 = vector<1>;
  using vec2 = vector<2>;
  using vec3 = vector<3>;
  using vec4 = vector<4>;

  using mat1x1 = matrix<1, 1>;
  using mat1x2 = matrix<1, 2>;
  using mat1x3 = matrix<1, 3>;
  using mat1x4 = matrix<1, 4>;
  using mat2x1 = matrix<2, 1>;
  using mat2x2 = matrix<2, 2>;
  using mat2x3 = matrix<2, 3>;
  using mat2x4 = matrix<2, 4>;
  using mat3x1 = matrix<3, 1>;
  using mat3x2 = matrix<3, 2>;
  using mat3x3 = matrix<3, 3>;
  using mat3x4 = matrix<3, 4>;
  using mat4x1 = matrix<4, 1>;
  using mat4x2 = matrix<4, 2>;
  using mat4x3 = matrix<4, 3>;
  using mat4x4 = matrix<4, 4>;

  template <typename T>
  using alloc = memory::arena_allocator<T>;

  template <natural_t R, natural_t C>
  static inline matrix_nxm shape_mat(const matrix<R, C>& vals) {
    return matrix_nxm::create(vals);
  }

  template <natural_t N>
  static inline dyn_vector shape_vec(const vector<N>& vals) {
    return dyn_vector{ vals };
  }

  template <typename T>
  static inline std::string as_string(const T& obj) {
    return core::as_string(obj);
  }

}  // namespace tensor

#endif  // TENSORLIB_HPP