/**
 * \file sandboxes/neural.cpp
 **/
#include "tensorlib.hpp"

// constexpr tensor::matrix<4, 2> or_inputs = {
//   {
//     std::array{ 0.f, 0.f },
//     std::array{ 0.f, 1.f },
//     std::array{ 1.f, 0.f },
//     std::array{ 1.f, 1.f },
//   }
// };

// constexpr tensor::vector<4> or_outputs = {
//   0.f,
//   1.f,
//   1.f,
//   1.f,
// };

int main() {
  // constexpr tensor::matrix<1, 1> weights{
  //   { std::array{ 0.5f } }
  // };
  // constexpr tensor::vector<1> input{ 1.f };
  // constexpr tensor::vector<1> bias{ 0.5f };

  // std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  // start = std::chrono::high_resolution_clock::now();

  // // Runtime version
  // // tensor::vector<1> runtime_output = tensor::neural::Layer<1, 1>(weights, bias).activate(input, &tensor::neural::sigmoid);

  // end = std::chrono::high_resolution_clock::now();
  // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  // // Print both results to verify they match
  // std::cout << "Runtime duration: " << elapsed.count() << " nanoseconds" << std::endl;

  // constexpr tensor::matrix<1, 1> weights{
  //   { std::array{ 0.5f } }
  // };
  // constexpr tensor::vector<1> bias{ 0.5f };
  // constexpr tensor::vector<1> input{ 1.f };

  // constexpr tensor::vector<1> layer_output = Layer<1>::apply_activation(weights, bias, input);

  // constexpr tensor::matrix<2, 2> mat1{
  //   {
  //     std::array{ 1.f, 2.f },
  //     std::array{ 3.f, 4.f },
  //   }
  // };
  // constexpr tensor::matrix<2, 2> mat2{
  //   {
  //     std::array{ 5.f, 6.f },
  //     std::array{ 7.f, 8.f },
  //   }
  // };
  // constexpr tensor::matrix<2, 2> mat3 = tensor::matrix_product(mat1, mat2);

  // static_assert(mat3(0, 0) == 19.f, "mat3(0,0) should be 19.");
  // static_assert(mat3(0, 1) == 22.f, "mat3(0,1) should be 22.");
  // static_assert(mat3(1, 0) == 43.f, "mat3(1,0) should be 43.");
  // static_assert(mat3(1, 1) == 50.f, "mat3(1,1) should be 50.");

  // constexpr tensor::matrix<2, 2> expected_res = {
  //   {
  //     std::array{ 19.f, 22.f },
  //     std::array{ 43.f, 50.f },
  //   }
  // };
  // static_assert(mat3 == expected_res, "Matrix multiplication result should be equal to expected result.");

  // tensor::vector<1> layer_output2 = Layer<1>(weights, bias, input).activate();

  // end = std::chrono::high_resolution_clock::now();

  // tensor::vector<3> test{ 1.f, 2.f, 3.f };
  // tensor::vector<3> test2{ 4.f, 5.f, 6.f };
  // tensor::vector<3> test_sum = tensor::vector_sum(test, test2);

  // std::cout << "Neuron output: " << layer_output[0] << std::endl;
  // std::cout << "Test sum: " << test_sum[0] << ", " << test_sum[1] << ", " << test_sum[2] << std::endl;
  // std::cout << "Elapsed time: " << elapsed.count() << " nanoseconds" << std::endl;

  return 0;
}