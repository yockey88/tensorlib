/**
 * \file detail/tensorlib_state.cpp
 **/
#include "detail/tensorlib_state.hpp"

#include <print>

namespace tensor {
  namespace detail {

    static void initialize_memory_arena() {
      std::println("Initializing memory arena");
      memory::allocate_arena(&tensorlib().main_arena);
    }

    static void deallocate_memory_arena() {
      std::println("Deallocating memory arena");
      memory::deallocate_arena(&tensorlib().main_arena);
    }

  }  // namespace detail

  tensorlib_state& tensorlib() {
    static tensorlib_state tensorlib_global_state;
    return tensorlib_global_state;
  }

  void init_tensor() {
    detail::initialize_memory_arena();
  }

  void shutdown_tensor() {
    detail::deallocate_memory_arena();
  }

}  // namespace tensor