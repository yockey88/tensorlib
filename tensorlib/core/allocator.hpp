/**
 * \file core/allocator.hpp
 **/
#ifndef TENSORLIB_CORE_ALLOCATOR_HPP
#define TENSORLIB_CORE_ALLOCATOR_HPP

namespace tensor {
  namespace memory {

    class allocator {
     public:
      virtual ~allocator() = default;
    };

  }  // namespace memory
}  // namespace tensor

#endif  // TENSORLIB_CORE_ALLOCATOR_HPP