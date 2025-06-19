/**
 * \file test_suites/tensorlib_testing.cpp
 **/
#include "tensorlib_testing.hpp"

#include "detail/tensorlib_state.hpp"

#include "core/arena.hpp"

namespace tensor {
  namespace testing {

    void tensorlib_test_environment::SetUp() {
      tensorlib().main_arena = subsystem<memory::arena>::get();
      memory::allocate_arena(tensorlib().main_arena);
    }

    void tensorlib_test_environment::TearDown() {
      memory::deallocate_arena(tensorlib().main_arena);
    }

    memory::arena* tensorlib_test_environment::get_memory_arena() {
      return tensorlib().main_arena;
    }

    namespace detail {
      namespace {

        struct test_metadata {
          tensorlib_test_environment* test_environment = nullptr;
        };

        static test_metadata& get_test_metadata() {
          static test_metadata metadata;
          return metadata;
        }

      }  // namespace
    }  // namespace detail

    void register_test_environment() {
      detail::get_test_metadata().test_environment = new tensorlib_test_environment();
      ::testing::AddGlobalTestEnvironment(detail::get_test_metadata().test_environment);
    }

    tensorlib_test_environment* get_test_environment() {
      return detail::get_test_metadata().test_environment;
    }

  }  // namespace testing
}  // namespace tensor