/**
 * \file test_suites/tensorlib_testing.hpp
 **/
#ifndef TENSORLIB_TESTING_HPP
#define TENSORLIB_TESTING_HPP

#include <gtest/gtest.h>

#include "core/arena.hpp"

#include "tensorlib.hpp"

namespace tensor {
  namespace testing {

    class tensorlib_test_environment : public ::testing::Environment {
     public:
      tensorlib_test_environment() = default;

      void SetUp() override;
      void TearDown() override;

      tensor::memory::arena* get_memory_arena();

     private:
      tensor::memory::arena memory_arena;
    };

    void register_test_environment();

    tensorlib_test_environment* get_test_environment();

  }  // namespace testing
}  // namespace tensor

#endif  // TENSORLIB_TESTING_HPP