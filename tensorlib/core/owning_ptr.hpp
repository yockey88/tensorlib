/**
 * \file core/scope.hpp
 **/
#ifndef TENSORLIB_CORE_SCOPE_HPP
#define TENSORLIB_CORE_SCOPE_HPP

#include "core/arena_allocator.hpp"

namespace tensor {

  template <typename T>
  struct owning_ptr_deleter {
    void operator()(T* ptr) const {
      if (ptr != nullptr) {
        memory::arena_allocator<T>{}.free(ptr);
      }
    }

    owning_ptr_deleter() = default;
    template <typename U>
      requires std::is_base_of_v<T, U>
    owning_ptr_deleter(const owning_ptr_deleter<U>&) {}
  };

  template <typename T>
  using owning_ptr = std::unique_ptr<T, owning_ptr_deleter<T>>;

  template <typename T, typename... Args>
    requires requires(Args&&... args) { std::declval<memory::arena_allocator<T>>().allocate(std::forward<Args>(args)...); }
  owning_ptr<T> make_owning_ptr(Args&&... args) {
    static memory::arena_allocator<T> allocator;
    T* ptr = allocator.allocate(std::forward<Args>(args)...);
    return std::unique_ptr<T, owning_ptr_deleter<T>>(ptr, owning_ptr_deleter<T>());
  }

}  // namespace tensor

#endif  // TENSORLIB_CORE_SCOPE_HPP