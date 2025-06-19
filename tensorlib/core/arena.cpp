/**
 * \file core/arena.cpp
 **/
#include "core/arena.hpp"

#include <cassert>
#include <print>

#include "core/defines.hpp"

namespace tensor {
  namespace memory {
    namespace detail {
      namespace {

        static void report_allocation(arena* arena, void* ptr, size_t size) {
          // PROFILE_ALLOCATION(memory, type_size);
          arena->bytes_allocated += size;
          arena->total_allocations++;
        }

        static void report_deallocation(arena* arena, void* ptr, size_t size) {
          // PROFILE_DEALLOCATION(ptr);
          arena->total_allocations--;
          arena->bytes_freed += size;
        }

        static void allocate_page(arena* arena_ptr, size_t page_index) {
          TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");
          TENSORLIB_ASSERT(page_index < arena::kMaxPages, "Page index out of bounds.");
          arena_ptr->pages[page_index] = (arena::page*)malloc(sizeof(arena::page));
          report_allocation(arena_ptr, arena_ptr->pages[page_index]->memory(), arena::kPageSize);
        }

        static void allocate_next_page(arena* arena_ptr) {
          TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");

          ++arena_ptr->page_allocation_cursor;
          TENSORLIB_ASSERT(arena_ptr->page_allocation_cursor < arena::kMaxPages, "Page index out of bounds.");

          allocate_page(arena_ptr, arena_ptr->page_allocation_cursor);

          arena_ptr->num_pages++;
          arena_ptr->page_cursor = 0;
        }

        static void deallocate_arena_page(arena* arena_ptr, size_t page_index) {
          TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");
          TENSORLIB_ASSERT(page_index < arena::kMaxPages, "Page index out of bounds.");

          report_deallocation(arena_ptr, arena_ptr->pages[page_index], arena::kPageSize);
          free(arena_ptr->pages[page_index]);
          arena_ptr->pages[page_index] = nullptr;

          arena_ptr->num_pages--;
        }

      }  // namespace
    }  // namespace detail

    arena::page::page() {
      std::ranges::fill(data, 0);
    }

    void* arena::page::memory(size_t offset) {
      TENSORLIB_ASSERT(offset < kPageSize, "Offset out of bounds for page memory.");
      return static_cast<void*>(data + offset);
    }

    natural_t arena::living_memory() const {
      return bytes_allocated - bytes_freed;
    }

    arena::page* arena::get_page(size_t index) {
      TENSORLIB_ASSERT(index < kMaxPages, "Page index out of bounds.");
      return pages[index];
    }

    void* arena::get_page_start(size_t index) {
      TENSORLIB_ASSERT(index < kMaxPages, "Page index out of bounds.");
      return pages[index]->memory();
    }

    void allocate_arena(arena* arena_ptr) {
      TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");

      std::println("Allocating [{}] size page for initial arena size", arena::kPageSize);
      detail::allocate_page(arena_ptr, 0);
      arena_ptr->page_allocation_cursor = 0;
      arena_ptr->num_pages = 1;
    }

    void deallocate_arena(arena* arena_ptr) {
      TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");

      std::println("Deallocating arena pages...");
      std::println(" -- Number of pages: {}", arena_ptr->num_pages);
      std::println(" -- Current Page Cursor: {}", arena_ptr->page_cursor);
      for (size_t i = 0; i < arena_ptr->num_pages; ++i) {
        detail::deallocate_arena_page(arena_ptr, i);
      }

      std::println(" -- Allocated Memory: {} bytes", arena_ptr->bytes_allocated);
      std::println(" -- Freed Memory: {} bytes", arena_ptr->bytes_freed);
      std::println(" -- Leaked Memory: {} bytes (cleaned up by arena)", arena_ptr->total_allocations, arena_ptr->living_memory());
      std::println("...Arena Deallocated");

      arena_ptr->page_allocation_cursor = 0;
      arena_ptr->page_cursor = 0;
      arena_ptr->bytes_allocated = 0;
      arena_ptr->bytes_freed = 0;
      arena_ptr->total_allocations = 0;
    }

    void* allocate_in_arena(arena* arena_ptr, size_t size, size_t alignment) {
      TENSORLIB_ASSERT(arena_ptr != nullptr, "Arena pointer cannot be null.");
      TENSORLIB_ASSERT(size < arena::kPageSize, "Allocation size is too large for Arena.");

      /// calculate alignment before checking size so that if alignment forces off a page we can allocate a new page
      /// and then align the pointer to the new page
      // clang-format off
      size_t alignment_offset = (arena_ptr->page_cursor % arena::kAlignment) != 0 ? 
        arena::kAlignment - (arena_ptr->page_cursor % arena::kAlignment) : 0;
      // clang-format on
      arena_ptr->page_cursor += alignment_offset;

      /// check if we need to allocate a new page, the first time page_cursor == 0 and size must be less than kPageSize
      ///   so this ensures that page_allocation_cursor won't move on the first allocation
      if (arena_ptr->page_cursor + size >= arena::kPageSize) {
        detail::allocate_next_page(arena_ptr);
        TENSORLIB_ASSERT(arena_ptr->page_cursor == 0, "Allocation size is too large for Arena.");
      }

      void* mem = arena_ptr->pages[arena_ptr->page_allocation_cursor]->memory(arena_ptr->page_cursor);
      TENSORLIB_ASSERT(mem != nullptr, "Memory allocation failed.");

      arena_ptr->page_cursor += size;

      detail::report_allocation(arena_ptr, mem, size);
      return mem;
    }

    void deallocate_from_arena(arena* arena_ptr, void* ptr, size_t size, size_t alignment) {
      if (ptr != nullptr) {
        memset(ptr, 0, size);
      }
      detail::report_deallocation(arena_ptr, ptr, size);
    }

  }  // namespace memory
}  // namespace tensor