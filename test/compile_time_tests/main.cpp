#include "linalgebra/vector.hpp"
#include "tensorlib.hpp"

int main() {
  {
    constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
    constexpr tensor::vector<3> p2{ 1.f, 2.f, 3.f };
    constexpr tensor::vector<3> p3{ 1.f, 2.f, 4.f };

    static_assert(p1 == p2, "Points should be equal.");
    static_assert(p1 != p3, "Points should not be equal.");
    static_assert(p1[0] == 1.f, "First coordinate should be 1.");
    static_assert(p1[1] == 2.f, "Second coordinate should be 2.");
    static_assert(p1[2] == 3.f, "Third coordinate should be 3.");
    static_assert(p1.size() == 3, "Point should have 3 dimensions.");
  }

  {
    constexpr tensor::vector<3> unit_point{ 1.f, 0.f, 0.f };
    constexpr tensor::real_t length = tensor::vector_magnitude(unit_point);

    static_assert(length == 1.f, "Magnitude should be 1.");
  }

  {
    constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
    constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };
    constexpr tensor::vector<3> sum = tensor::vector_sum(p1, p2);
    constexpr tensor::vector<3> diff = tensor::vector_difference(p1, p2);

    static_assert(sum[0] == 5.f && sum[1] == 7.f && sum[2] == 9.f, "Sum should be (5, 7, 9).");
    static_assert(diff[0] == -3.f && diff[1] == -3.f && diff[2] == -3.f, "Difference should be (-3, -3, -3).");
  }

  {
    constexpr tensor::vector<3> p1{ 1.f, 1.f, 1.f };
    constexpr tensor::real_t scalar = 2.f;
    constexpr tensor::vector<3> scaled_p1 = tensor::scalar_product(scalar, p1);

    constexpr tensor::vector<3> p2{ 2.f, 2.f, 2.f };
    static_assert(scaled_p1 == p2, "Scaled point should be (2, 2, 2).");
  }

  {
    constexpr tensor::vector<3> p1{ 1.f, 2.f, 3.f };
    constexpr tensor::vector<3> p2{ 4.f, 5.f, 6.f };
    constexpr tensor::real_t dot_product = tensor::dot_product(p1, p2);

    static_assert(dot_product == 32.f, "Dot product should be 32.");
  }

  {
    constexpr tensor::matrix<1, 1> m1{ 1.f };
    constexpr tensor::matrix<1, 1> m2{ 1.f };

    constexpr tensor::matrix<2, 2> m3{
      {
        std::array{ 1.f, 2.f },
        std::array{ 3.f, 4.f },
      }
    };
    constexpr tensor::matrix<2, 2> m4{ 5.f, 6.f, 7.f, 8.f };

    /// (i, j) -> a_ij
    static_assert(m3(0, 0) == 1.f, "m3(0,0) should be 1.");
    static_assert(m3(1, 0) == 3.f, "m3(1,0) should be 3.");
    static_assert(m3(0, 1) == 2.f, "m3(0,1) should be 2.");
    static_assert(m3(1, 1) == 4.f, "m3(0,0) should be 4.");

    static_assert(m1 == m2, "Matrices should be equal.");
    static_assert(m1 != m3, "Matrices should not be equal.");
    static_assert(m3 != m4, "Matrices should not be equal.");

    /// [i][j] -> row vector ([1][0] = 2 = m3(0,1) == m3_01)
    // static_assert(m3[0][0] == 1.f, "First element should be 1.");
    // static_assert(m3[1][0] == 2.f, "Fourth element should be 4.");
    // static_assert(m3[0][1] == 3.f, "Third element should be 3.");
    // static_assert(m3[1][1] == 4.f, "Second element should be 2.");
    static_assert(m3.size() == 4, "Matrix should have 4 elements.");
    static_assert(m3.size() == m3.data_size, "Matrix size() should match data size.");
    static_assert(m3.rows == 2, "Matrix should have 2 rows.");
    static_assert(m3.cols == 2, "Matrix should have 2 columns.");

    constexpr tensor::matrix<3, 3> m5{
      {
        std::array{ 1.f, 2.f, 3.f },
        std::array{ 4.f, 5.f, 6.f },
        std::array{ 7.f, 8.f, 9.f },
      }
    };

    static_assert(m5(0, 0) == 1.f, "m5(0,0) should be 1.");
    static_assert(m5(1, 0) == 4.f, "m5(1,0) should be 4.");
    static_assert(m5(0, 1) == 2.f, "m5(0,1) should be 2.");
    static_assert(m5(1, 1) == 5.f, "m5(1,1) should be 5.");
    static_assert(m5(0, 2) == 3.f, "m5(0,2) should be 3.");
    static_assert(m5(1, 2) == 6.f, "m5(1,2) should be 6.");
    static_assert(m5(2, 0) == 7.f, "m5(2,0) should be 7.");
    static_assert(m5(2, 1) == 8.f, "m5(2,1) should be 8.");
    static_assert(m5(2, 2) == 9.f, "m5(2,2) should be 9.");
  }

  {
    constexpr tensor::matrix<3, 3> id{
      {
        std::array{ 1.f, 0.f, 0.f },
        std::array{ 0.f, 1.f, 0.f },
        std::array{ 0.f, 0.f, 1.f },
      }
    };

    static_assert(id(0, 0) == 1.f, "id(0,0) should be 1.");
    static_assert(id(1, 0) == 0.f, "id(1,0) should be 0.");
    static_assert(id(0, 1) == 0.f, "id(0,1) should be 0.");
    static_assert(id(1, 1) == 1.f, "id(1,1) should be 1.");
    static_assert(id(0, 2) == 0.f, "id(0,2) should be 0.");
    static_assert(id(1, 2) == 0.f, "id(1,2) should be 0.");
    static_assert(id(2, 0) == 0.f, "id(2,0) should be 0.");
    static_assert(id(2, 1) == 0.f, "id(2,1) should be 0.");
    static_assert(id(2, 2) == 1.f, "id(2,2) should be 1.");

    static_assert(tensor::identity<3>(0, 0) == 1.f, "id(0,0) should be 1.");
    static_assert(tensor::identity<3>(1, 0) == 0.f, "id(1,0) should be 0.");
    static_assert(tensor::identity<3>(0, 1) == 0.f, "id(0,1) should be 0.");
    static_assert(tensor::identity<3>(1, 1) == 1.f, "id(1,1) should be 1.");
    static_assert(tensor::identity<3>(0, 2) == 0.f, "id(0,2) should be 0.");
    static_assert(tensor::identity<3>(1, 2) == 0.f, "id(1,2) should be 0.");
    static_assert(tensor::identity<3>(2, 0) == 0.f, "id(2,0) should be 0.");
    static_assert(tensor::identity<3>(2, 1) == 0.f, "id(2,1) should be 0.");
    static_assert(tensor::identity<3>(2, 2) == 1.f, "id(2,2) should be 1.");
  }

  {
    constexpr tensor::matrix<3, 3> expected_sum{
      {
        std::array{ 2.f, 0.f, 0.f },
        std::array{ 0.f, 2.f, 0.f },
        std::array{ 0.f, 0.f, 2.f },
      }
    };
    static_assert(tensor::matrix_sum(tensor::identity<3>, tensor::identity<3>) == expected_sum, "Matrix sum should be equal to expected sum.");
  }

  {
    constexpr tensor::matrix<3, 3> m1{
      {
        std::array{ 1.f, 1.f, 1.f },
        std::array{ 2.f, 2.f, 2.f },
        std::array{ 3.f, 3.f, 3.f },
      }
    };

    constexpr tensor::matrix<3, 3> m2{
      {
        std::array{ 4.f, 4.f, 4.f },
        std::array{ 5.f, 5.f, 5.f },
        std::array{ 6.f, 6.f, 6.f },
      }
    };

    constexpr tensor::matrix<3, 3> expected_product{
      {
        std::array{ 15.f, 15.f, 15.f },
        std::array{ 30.f, 30.f, 30.f },
        std::array{ 45.f, 45.f, 45.f },
      }
    };

    static_assert(tensor::matrix_product(m1, m2) == expected_product, "Matrix product should be equal to expected product.");
  }

  {
    constexpr tensor::matrix<3, 2> m1{
      {
        std::array{ 1.f, 1.f },
        std::array{ 2.f, 2.f },
        std::array{ 3.f, 3.f },
      }
    };
    constexpr tensor::matrix<2, 3> m2{
      {
        std::array{ 1.f, 1.f, 1.f },
        std::array{ 2.f, 2.f, 2.f },
      }
    };

    constexpr tensor::matrix<3, 3> expected_product{
      {
        std::array{ 3.f, 3.f, 3.f },
        std::array{ 6.f, 6.f, 6.f },
        std::array{ 9.f, 9.f, 9.f },
      }
    };

    static_assert(tensor::matrix_product(m1, m2) == expected_product, "Matrix product should be equal to expected product.");
  }

  {
    constexpr tensor::matrix<1, 1> m1{ 1.f };
    constexpr tensor::matrix<1, 1> m2{ 2.f };

    constexpr tensor::matrix<1, 1> expected_product{ 2.f };
    static_assert(tensor::matrix_product(m1, m2) == expected_product, "Matrix product should be equal to expected product.");
  }

  {
    static_assert(tensor::detail::epsilon_equal(0.f, 0.f), "0 should be equal to 0.");
    static_assert(!tensor::detail::epsilon_equal(0.f, 0.0001f), "0 should be equal to 0.0001.");
    static_assert(tensor::detail::epsilon_equal(0.f, 0.0001f, 0.0002f), "0 should be equal to 0.0001 with epsilon 0.0002.");
    static_assert(!tensor::detail::epsilon_equal(0.f, 0.0001f, 0.00005f), "0 should not be equal to 0.0001 with epsilon 0.00005.");

    static_assert(tensor::detail::epsilon_equal(tensor::neural::detail::sigmoid(-3.f), 0.0474258773f, 0.0001f), "sigmoid activation function should return 0.0474258773f for input -3.");
    static_assert(tensor::detail::epsilon_equal(tensor::neural::detail::sigmoid(-3.f), 0.0474f, 0.0001f), "sigmoid activation function should return 0.0474f for input -3 with epsilon 0.0001.");
    static_assert(tensor::neural::detail::sigmoid(3.f) == 0.952574126822433f, "sigmoid activation function should return 0.952574126822433f for input 3.");
    static_assert(tensor::detail::epsilon_equal(tensor::neural::detail::sigmoid(3.f), 0.9526f, 0.0001f), "sigmoid activation function should return 0.9526f for input 3 with epsilon 0.0001.");
    static_assert(tensor::neural::detail::sigmoid(0.f) == 0.5f, "sigmoid activation function should return 0.5 for input 0.");

    static_assert(tensor::neural::detail::relu(-3.f) == 0.f, "relu activation function should return 0 for input -3.");
    static_assert(tensor::neural::detail::relu(3.f) == 3.f, "relu activation function should return 3 for input 3.");
    static_assert(tensor::neural::detail::relu(0.f) == 0.f, "relu activation function should return 0 for input 0.");
    static_assert(tensor::neural::detail::relu(0.5f) == 0.5f, "relu activation function should return 0.5 for input 0.5.");
    static_assert(tensor::neural::detail::relu(-0.5f) == 0.f, "relu activation function should return 0 for input -0.5.");
    static_assert(tensor::neural::detail::relu(1.5f) == 1.5f, "relu activation function should return 1.5 for input 1.5.");
  }

  {
    constexpr tensor::vector<3> v1{ 1.f, 2.f, 3.f };
    constexpr tensor::matrix<3, 3> m1{
      {
        std::array{ 1.f, 2.f, 3.f },
        std::array{ 4.f, 5.f, 6.f },
        std::array{ 7.f, 8.f, 9.f },
      }
    };

    constexpr tensor::vector<3> expected_product{ 14.f, 32.f, 50.f };
    constexpr tensor::vector<3> product = tensor::matrix_vector_product(m1, v1);
    static_assert(product == expected_product, "Matrix-vector product should be equal to expected product.");
  }

  return 0;
}