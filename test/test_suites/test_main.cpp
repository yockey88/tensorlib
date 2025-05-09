/**
 * \file test_suites/test_main.cpp
 **/
#include <gtest/gtest.h>

#include "detail/tensorlib_state.hpp"

#include "tensorlib.hpp"
#include "tensorlib_testing.hpp"

int main(int argc, char** argv) {
  tensor::testing::register_test_environment();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}