/**
 * \file detail/tensorlib_state.hpp
 **/
#ifndef TENSORLIB_DETAIL_TENSORLIB_STATE_HPP
#define TENSORLIB_DETAIL_TENSORLIB_STATE_HPP

#include "core/arena.hpp"

namespace tensor {

  struct tensorlib_state {
    memory::arena main_arena;
  };

  tensorlib_state& tensorlib();

  void init_tensor();
  void shutdown_tensor();

}  // namespace tensor

#endif  // TENSORLIB_DETAIL_TENSORLIB_STATE_HPP