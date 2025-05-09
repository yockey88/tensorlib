/*
 * \file test_suites/memory_tests/buffer_tests.cpp
 **/
#include <gtest/gtest.h>

#include "core/buffer.hpp"

class BufferTests : public ::testing::Test {
};

TEST_F(BufferTests, basic_buffer) {
  tensor::buffer<int> buf;
  EXPECT_EQ(buf.get_capacity(), 0);

  buf.resize(10);
  EXPECT_EQ(buf.get_capacity(), 10);
  for (size_t i = 0; i < buf.get_capacity(); ++i) {
    buf[i] = static_cast<int>(i);
  }

  for (size_t i = 0; i < buf.get_capacity(); ++i) {
    EXPECT_EQ(buf[i], static_cast<int>(i));
  }
}

TEST_F(BufferTests, buffer_initialization) {
  tensor::buffer<int> buf{ 1, 2, 3, 4, 5 };
  EXPECT_EQ(buf.get_capacity(), 5);
  for (size_t i = 0; i < buf.get_capacity(); ++i) {
    EXPECT_EQ(buf[i], static_cast<int>(i + 1));
  }
}