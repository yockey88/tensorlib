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

    class TensorLibEnvironment : public ::testing::Environment {
     public:
      TensorLibEnvironment() = default;

      void SetUp() override;
      void TearDown() override;

      tensor::memory::arena* get_memory_arena();

     private:
      tensor::memory::arena memory_arena;
    };

    void register_test_environment();

    TensorLibEnvironment* get_test_environment();

  }  // namespace testing
}  // namespace tensor

#endif  // TENSORLIB_TESTING_HPP