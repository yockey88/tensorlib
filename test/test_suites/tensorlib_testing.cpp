/**
 * \file test_suites/tensorlib_testing.cpp
 **/
#include "tensorlib_testing.hpp"

#include "core/arena.hpp"
#include "detail/tensorlib_state.hpp"

namespace tensor {
  namespace testing {

    void TensorLibEnvironment::SetUp() {
      tensor::memory::allocate_arena(&memory_arena);
      tensor::tensorlib().main_arena = memory_arena;
    }

    void TensorLibEnvironment::TearDown() {
      tensor::memory::deallocate_arena(&memory_arena);
    }

    tensor::memory::arena* TensorLibEnvironment::get_memory_arena() {
      return &memory_arena;
    }

    namespace detail {
      namespace {

        struct test_metadata {
          TensorLibEnvironment* test_environment = nullptr;
        };

        static test_metadata& get_test_metadata() {
          static test_metadata metadata;
          return metadata;
        }

      }  // namespace
    }  // namespace detail

    void register_test_environment() {
      TensorLibEnvironment* env = new TensorLibEnvironment();
      ::testing::AddGlobalTestEnvironment(env);

      detail::get_test_metadata().test_environment = env;
    }

    TensorLibEnvironment* get_test_environment() {
      return detail::get_test_metadata().test_environment;
    }

  }  // namespace testing
}  // namespace tensor