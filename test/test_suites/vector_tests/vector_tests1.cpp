/**
 * \file test_suites/vector_tests/vector_tests1.cpp
 **/
#include <gtest/gtest.h>

#include "tensorlib.hpp"

class VectorTests : public ::testing::Test {
 protected:
};

TEST_F(VectorTests, VectorCreation) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };

  EXPECT_EQ(p1[0], 1.f);
  EXPECT_EQ(p1[1], 2.f);
  EXPECT_EQ(p1[2], 3.f);
  EXPECT_EQ(p2[0], 4.f);
  EXPECT_EQ(p2[1], 5.f);
  EXPECT_EQ(p2[2], 6.f);
}

TEST_F(VectorTests, VectorEquality) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p2{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p3{ 1.f, 2.f, 4.f };

  EXPECT_EQ(p1, p2);
  EXPECT_NE(p1, p3);
}

TEST_F(VectorTests, VectorSize) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  EXPECT_EQ(p1.size(), 3);
}

TEST_F(VectorTests, VectorMagnitude) {
  constexpr tensor::vector<4> unit_point{ 1.f, 0.f, 0.f, 0.f };
  constexpr tensor::real_t length = tensor::vector_magnitude(unit_point);
  EXPECT_EQ(length, 1.f);
}

TEST_F(VectorTests, VectorSum) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };
  constexpr tensor::vector<3> sum = tensor::vector_sum(p1, p2);
  EXPECT_EQ(sum[0], 5.f);
  EXPECT_EQ(sum[1], 7.f);
  EXPECT_EQ(sum[2], 9.f);
}

TEST_F(VectorTests, VectorDifference) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };
  constexpr tensor::vector<3> diff = tensor::vector_difference(p1, p2);
  EXPECT_EQ(diff[0], -3.f);
  EXPECT_EQ(diff[1], -3.f);
  EXPECT_EQ(diff[2], -3.f);
}

TEST_F(VectorTests, VectorScalarProduct) {
  constexpr tensor::vector<3> p1{ 1.f, 1.f, 1.f };
  constexpr tensor::real_t scalar = 2.f;
  constexpr tensor::vector<3> scaled_p1 = tensor::scalar_product(scalar, p1);
  EXPECT_EQ(scaled_p1[0], 2.f);
  EXPECT_EQ(scaled_p1[1], 2.f);
  EXPECT_EQ(scaled_p1[2], 2.f);
}

TEST_F(VectorTests, VectorDotProduct) {
  constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
  constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };
  constexpr tensor::real_t dot_product = tensor::dot_product(p1, p2);
  EXPECT_EQ(dot_product, 32.f);
}