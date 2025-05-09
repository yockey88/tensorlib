/**
 * \file core/arena_allocator.hpp
 **/
#ifndef TENSORLIB_CORE_ARENA_ALLOCATOR_HPP
#define TENSORLIB_CORE_ARENA_ALLOCATOR_HPP

#include "core/allocator.hpp"
#include "core/arena.hpp"
#include "detail/tensorlib_state.hpp"

namespace tensor {
  namespace memory {

    template <typename T>
    class arena_allocator : public allocator {
     public:
      using value_type = T;

      constexpr static inline bool call_dtor = !std::is_trivially_destructible_v<T>;

      arena_allocator() {}
      arena_allocator(arena* arena)
          : overide_arena(arena) {}
      arena_allocator(arena& arena)
          : overide_arena(&arena) {}
      virtual ~arena_allocator() override = default;

      template <typename U>
      constexpr arena_allocator(const arena_allocator<U>&) noexcept {}

      template <typename... Args>
      T* allocate(Args&&... args) {
        /// TODO: custom alignment
        void* memory = allocate_in_arena(get_arena(), type_size);
        if (memory == nullptr) {
          TENSORLIB_ASSERT(false, "Memory allocation failed.");
          throw std::bad_alloc();
        }

        if (sizeof...(args) > 0) {
          new (memory) T(std::forward<Args>(args)...);
        } else {
          new (memory) T();
        }

        return std::launder(static_cast<T*>(memory));
      }

      void free(T* ptr) {
        if (ptr != nullptr) {
          if constexpr (call_dtor) {
            ptr->~T();
          }
        }
        deallocate_from_arena(get_arena(), ptr, type_size);
      }

      /// chunks == arrays of T
      T* allocate_chunk(size_t size) {
        if (size == 0) {
          return nullptr;
        }

        void* memory = allocate_in_arena(get_arena(), size * type_size);
        if (memory == nullptr) {
          TENSORLIB_ASSERT(false, "Memory allocation failed.");
          throw std::bad_alloc();
        }

        // PROFILE_ALLOCATION(memory, type_size);
        T* ptr = static_cast<T*>(memory);
        for (size_t i = 0; i < size; ++i) {
          new (ptr + i) T();
        }

        return std::launder(static_cast<T*>(memory));
      }

      void free_chunk(T* ptr, size_t sz) noexcept {
        if (ptr != nullptr) {
          for (size_t i = 0; i < sz; ++i) {
            if constexpr (call_dtor) {
              ptr[i].~T();
            }
          }
        }

        deallocate_from_arena(get_arena(), ptr, sz * type_size);
      }

      static constexpr size_t type_size = sizeof(T);

     private:
      arena* overide_arena = nullptr;

      arena* get_arena() {
        if (overide_arena != nullptr) {
          return overide_arena;
        } else {
          return &tensorlib().main_arena;
        }
      }
    };

  }  // namespace memory
}  // namespace tensor

#endif  // TENSORLIB_CORE_ARENA_ALLOCATOR_HPP