/**
 * \file test_suites/test_main.cpp
 **/
#include <gtest/gtest.h>

#include "tensorlib.hpp"

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}