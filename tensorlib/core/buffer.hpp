/**
 * \file core/buffer.hpp
 **/
#ifndef TENSORLIB_CORE_BUFFER_HPP
#define TENSORLIB_CORE_BUFFER_HPP

#include <cstddef>
#include <stdexcept>

#include "core/arena_allocator.hpp"
#include "core/ref.hpp"

namespace tensor {

  template <typename T>
  struct buffer_storage : public ref_counted {
    buffer_storage() = default;
    buffer_storage(size_t capacity)
        : capacity(capacity) {
      // data = allocator.allocate_chunk(capacity);
    }
    ~buffer_storage() {
      // allocator.free_chunk(data, capacity);
      capacity = 0;
    }

    const T& at(size_t index) const {
      if (data == nullptr) {
        throw std::runtime_error("Buffer is not initialized.");
      }
      if (index >= capacity) {
        throw std::out_of_range("Index out of range");
      }
      return data[index];
    }

    T& at(size_t index) {
      if (data == nullptr) {
        throw std::runtime_error("Buffer is not initialized.");
      }
      if (index >= capacity) {
        throw std::out_of_range("Index out of range");
      }
      return data[index];
    }

    memory::arena_allocator<T> allocator;
    size_t capacity = 0;

   private:
    T* data = nullptr;
  };

  template <typename T>
    requires requires(T) { T{}; }
  class buffer {
    ref<buffer_storage<T>> storage = nullptr;

   public:
    buffer() {
      storage = make_ref<buffer_storage<T>>();
    }

    buffer(size_t capacity) {
      TENSORLIB_ASSERT(capacity > 0, "Buffer capacity must be greater than zero.");
      storage = make_ref<buffer_storage<T>>(capacity);
    }

    buffer(const std::initializer_list<T>& init_list) {
      TENSORLIB_ASSERT(init_list.size() > 0, "Buffer capacity must be greater than zero.");
      storage = make_ref<buffer_storage<T>>(init_list.size());
      for (size_t i = 0; i < init_list.size(); ++i) {
        storage->at(i) = *(init_list.begin() + i);
      }
    }

    buffer(const buffer& other) {
      storage = other.storage;
    }
    buffer& operator=(const buffer& other) {
      if (this != &other) {
        storage = other.storage;
      }
      return *this;
    }

    buffer(buffer&& other) noexcept {
      storage = other.storage;
      other.storage = nullptr;
    }
    buffer& operator=(buffer&& other) noexcept {
      if (this != &other) {
        other.storage = nullptr;
      }
      return *this;
    }

    ~buffer() {
      storage = nullptr;
    }

    void resize(size_t new_size) {
      storage = make_ref<buffer_storage<T>>(new_size);
    }

    T& operator[](size_t index) {
      return storage->at(index);
    }

    const T& operator[](size_t index) const {
      return storage->at(index);
    }

    size_t get_capacity() const { return storage->capacity; }
  };

}  // namespace tensor

#endif  // TENSORLIB_CORE_BUFFER_HPP