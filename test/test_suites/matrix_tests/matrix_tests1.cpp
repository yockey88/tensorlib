/**
 * \file test_suites/matrix_tests/matrix_tests1.cpp
 **/
#include <gtest/gtest.h>

#include "tensorlib.hpp"

class MatrixTests : public ::testing::Test {
 protected:
};

TEST_F(MatrixTests, MatrixCreation) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m2{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };

  EXPECT_EQ(m1(0, 0), 1.f);
  EXPECT_EQ(m1(0, 1), 2.f);
  EXPECT_EQ(m1(0, 2), 3.f);

  EXPECT_EQ(m1(1, 0), 4.f);
  EXPECT_EQ(m1(1, 1), 5.f);
  EXPECT_EQ(m1(1, 2), 6.f);

  EXPECT_EQ(m1(2, 0), 7.f);
  EXPECT_EQ(m1(2, 1), 8.f);
  EXPECT_EQ(m1(2, 2), 9.f);

  EXPECT_EQ(m2(0, 0), 1.f);
  EXPECT_EQ(m2(0, 1), 2.f);
  EXPECT_EQ(m2(0, 2), 3.f);

  EXPECT_EQ(m2(1, 0), 4.f);
  EXPECT_EQ(m2(1, 1), 5.f);
  EXPECT_EQ(m2(1, 2), 6.f);
}

TEST_F(MatrixTests, MatrixEquality) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m2{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m3{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 2.f, 5.f, 6.f },
      std::array{ 3.f, 6.f, 0.f },
    }
  };

  EXPECT_EQ(m1, m2);
  EXPECT_NE(m1, m3);
}

TEST_F(MatrixTests, MatrixSize) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  EXPECT_EQ(m1.rows, 3);
  EXPECT_EQ(m1.cols, 3);

  constexpr tensor::matrix<2, 4> m2{
    {
      std::array{ 1.f, 2.f, 3.f, 4.f },
      std::array{ 5.f, 6.f, 7.f, 8.f },
    }
  };
  EXPECT_EQ(m2.rows, 2);

  ASSERT_EQ(m2(0, 0), 1.f);
  ASSERT_EQ(m2(0, 1), 2.f);
  ASSERT_EQ(m2(0, 2), 3.f);
  ASSERT_EQ(m2(0, 3), 4.f);
  ASSERT_EQ(m2(1, 0), 5.f);
  ASSERT_EQ(m2(1, 1), 6.f);
  ASSERT_EQ(m2(1, 2), 7.f);
  ASSERT_EQ(m2(1, 3), 8.f);
}

TEST_F(MatrixTests, MatrixSum) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m2{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> sum = tensor::matrix_sum(m1, m2);

  EXPECT_EQ(sum(0, 0), 2.f);
  EXPECT_EQ(sum(0, 1), 4.f);
}

TEST_F(MatrixTests, MatrixDifference) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m2{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> diff = tensor::matrix_difference(m1, m2);
  constexpr tensor::matrix<3, 3> zero_mat = tensor::zero_matrix<3, 3>;

  EXPECT_EQ(diff, zero_mat);
}

TEST_F(MatrixTests, MatrixProduct) {
  constexpr tensor::matrix<3, 3> m1{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> m2{
    {
      std::array{ 1.f, 2.f, 3.f },
      std::array{ 4.f, 5.f, 6.f },
      std::array{ 7.f, 8.f, 9.f },
    }
  };
  constexpr tensor::matrix<3, 3> product = tensor::matrix_product(m1, m2);

  EXPECT_EQ(product(0, 0), 30.f);
}