/**
 * \file detail/tensorlib_state.hpp
 **/
#ifndef TENSORLIB_DETAIL_TENSORLIB_STATE_HPP
#define TENSORLIB_DETAIL_TENSORLIB_STATE_HPP

namespace tensor {

  namespace memory {
    struct arena;
  }
  class logger;
  struct tensorlib_state {
    memory::arena* main_arena;
    logger* main_logger;
  };

  tensorlib_state& tensorlib();

  void init_tensor();
  void shutdown_tensor();

}  // namespace tensor

#endif  // TENSORLIB_DETAIL_TENSORLIB_STATE_HPP