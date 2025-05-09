/**
 * \file core/ref.cpp
 **/
#include "core/ref.hpp"

#include <mutex>
#include <unordered_set>

namespace tensor {

  void ref_counted::view_increment() const {
    ++views;
  }

  void ref_counted::increment() {
    ++active_refs;
  }

  void ref_counted::view_decrement() const {
    --views;
  }

  void ref_counted::decrement() {
    --active_refs;
  }

  uint64_t ref_counted::view_count() const {
    return views.load();
  }

  uint64_t ref_counted::count() const {
    return active_refs.load();
  }

  namespace detail {

    static std::mutex mutex;
    std::unordered_set<void*> references;

    void register_reference(void* instance) {
      std::lock_guard lock(mutex);
      references.insert(instance);
    }

    void remove_reference(void* instance) {
      references.erase(instance);
    }

    bool is_valid_ref(void* instance) {
      return references.find(instance) != references.end();
    }

    size_t number_of_living_references() {
      return references.size();
    }

  }  // namespace detail
}  // namespace tensor