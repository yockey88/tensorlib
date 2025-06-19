/**
 * \file core/arena.hpp
 **/
#ifndef TENSORLIB_CORE_ARENA_HPP
#define TENSORLIB_CORE_ARENA_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>

#include "core/subsystem.hpp"
#include "core/types.hpp"

namespace tensor {
  namespace memory {

    using dtor_fn_t = void (*)(void*, size_t, size_t);

    struct chunk {
      void* address = nullptr;
      size_t offset = 0;
      size_t size = 0;
    };

    struct arena : public subsystem<arena> {
      static inline constexpr size_t kPageSize = 64 * 4096u * 4096u;
      /// TODO: this feels wrong, this is correct for the GPU but seems incorrect if
      ///         aiming to be as cross platform as possible (research: confirm all GPUs read mem in 16 byte chunks)
      static inline constexpr size_t kAlignment = 16;
      static inline constexpr size_t kMaxPages = 16;
      static inline constexpr size_t kMaxMemoryAllowed = kMaxPages * kPageSize;

      struct page {
        alignas(kAlignment) uint8_t data[kPageSize];

        page();
        void* memory(size_t offset = 0);
      };

      size_t num_pages = 0;
      size_t page_allocation_cursor = 0;
      size_t page_cursor = 0;

      size_t total_allocations = 0;
      size_t bytes_allocated = 0;
      size_t bytes_freed = 0;

      page* pages[kMaxPages] = {};

      natural_t living_memory() const;

      page* get_page(size_t index);
      void* get_page_start(size_t index);

      static void* allocate(size_t size);
      static void free(void* ptr, size_t size);
    };

    void allocate_arena(arena* arena_ptr);
    void deallocate_arena(arena* arena_ptr);

    void* allocate_in_arena(arena* arena_ptr, size_t size, size_t alignment = arena::kAlignment);
    void deallocate_from_arena(arena* arena_ptr, void* ptr, size_t size, size_t alignment = arena::kAlignment);

  }  // namespace memory
}  // namespace tensor

TENSORLIB_SUBSYSTEM(tensor::memory::arena);

#endif  // TENSORLIB_CORE_ARENA_HPP