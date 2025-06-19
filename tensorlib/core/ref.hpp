/**
 * \file core/ref.hpp
 **/
#ifndef TENSORLIB_CORE_REF_HPP
#define TENSORLIB_CORE_REF_HPP

#include <atomic>
#include <concepts>

#include "core/arena.hpp"
#include "core/arena_allocator.hpp"

namespace tensor {

  class ref_counted {
   public:
    ref_counted()
        : views(0), active_refs(0) {}
    virtual ~ref_counted() = default;

    void view_increment() const;
    void view_decrement() const;

    void increment();
    void decrement();

    uint64_t view_count() const;
    uint64_t count() const;

   private:
    mutable std::atomic<uint64_t> views;
    mutable std::atomic<uint64_t> active_refs;
  };

  namespace detail {

    void register_reference(void* instance);
    void remove_reference(void* instance);
    bool is_valid_ref(void* instance);
    size_t number_of_living_references();

  }  // namespace detail

  template <typename T, typename U>
  concept ref_castable = std::convertible_to<T, U> || std::derived_from<T, U> || std::derived_from<U, T>;

  template <typename T>
  concept ref_type = std::derived_from<T, ref_counted>;

  template <typename T>
  class ref {
   public:
    ref() : arena_ptr(nullptr), object(nullptr) {}

    ref(T* p) {
      object = p;
      inc_ref();
    }

    ref(const ref<T>& other) {
      object = other.object;
      inc_ref();
    }

    /// ref count stays the same under move assignment
    ref(ref<T>&& other) noexcept {
      object = other.object;
      other.object = nullptr;
    }

    ref& operator=(const ref<T>& other) {
      if (this != &other) {
        object = other.object;
        inc_ref();
      }
      return *this;
    }

    /// ref count stays the same under move assignment
    ref& operator=(ref<T>&& other) noexcept {
      if (this != &other) {
        object = other.object;
        other.object = nullptr;
      }
      return *this;
    }

    template <typename T2>
    ref(const ref<T2>& other) {
      object = (T*)other.object;
      inc_ref();
    }

    /// ref count stays the same under move assignment
    template <typename T2>
    ref(ref<T2>&& other) noexcept {
      object = (T*)other.object;
      other.object = nullptr;
    }

    virtual ~ref() {
      dec_ref();
    }

    template <typename T2>
    ref& operator=(const ref<T2>& other) {
      static_assert(std::is_base_of_v<T, T2>, "No viable conversion to construct ref with");
      other.inc_ref();
      dec_ref();

      object = reinterpret_cast<T*>(other.object);
      return *this;
    }

    /// ref count stays the same under move assignment
    template <typename T2>
    ref& operator=(ref<T2>&& other) noexcept {
      static_assert(std::is_base_of_v<T, T2>, "No viable conversion to construct ref with");

      object = reinterpret_cast<T*>(other.object);
      other.object = nullptr;

      return *this;
    }

    ref& operator=(std::nullptr_t) {
      dec_ref();
      object = nullptr;
      return *this;
    }

    operator bool() { return object != nullptr; }
    operator bool() const { return object != nullptr; }

    T& operator*() { return *object; }
    const T& operator*() const { return *object; }

    T* operator->() { return object; }
    const T* operator->() const { return object; }

    T* Raw() { return object; }
    const T* Raw() const { return object; }

    template <typename U>
      requires ref_castable<T, U>
    static ref<T> clone(const ref<U>& old_ref) {
      if constexpr (std::same_as<T, U>) {
        return ref<T>(old_ref);
      } else {
        return ref<T>(reinterpret_cast<T*>(old_ref.object));
      }

      /// Unreachable
      throw invalid_ref_cast(typeid(T), typeid(U));
    }

    template <typename... Args>
      requires std::is_base_of_v<ref_counted, T> &&
      requires(Args&&... args) { memory::arena_allocator<T>{}.allocate(std::forward<Args>(args)...); }
    static ref<T> create(Args&&... args) {
      static memory::arena_allocator<T> allocator{};
      return ref<T>(allocator.allocate(std::forward<Args>(args)...));
    }

    bool operator==(const ref<T>& other) const {
      return object == other.object;
    }

    bool operator==(std::nullptr_t) const {
      return object == nullptr;
    }

    bool equals_obj(const ref<T>& other) const {
      return object == other.object;
    }

   private:
    static inline memory::arena_allocator<T> allocator;

    /// requires mutable to call IncRef and DecRef in const contexts
    mutable memory::arena* arena_ptr;
    mutable T* object;

    /// for direct referencing in cases where we don't want to increment the reference count
    ref(T* p, bool) {
      object = p;
    }

    void inc_ref() const {
      if (object != nullptr) {
        object->increment();
        detail::register_reference(object);
      }
    }

    void dec_ref() const {
      if (object != nullptr) {
        object->decrement();

        if (object->count() == 0) {
          TENSORLIB_ASSERT(detail::is_valid_ref(object), "Invalid reference detected.");
          TENSORLIB_ASSERT(detail::number_of_living_references() > 0, "No living references detected to remove.");

          detail::remove_reference(object);
          allocator.free(object);
          object = nullptr;
        }
      }
    }

    template <typename U>
    friend class ref;
  };

  template <typename T, typename... Args>
    requires ref_type<T> && std::constructible_from<T, Args...>
  ref<T> make_ref(Args&&... args) {
    return ref<T>::create(std::forward<Args>(args)...);
  }

}  // namespace tensor

#endif  // TENSORLIB_CORE_REF_HPP