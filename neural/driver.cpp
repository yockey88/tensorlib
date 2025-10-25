/**
 * \file neural/driver.cpp
 **/
#include <print>

#include "core/types.hpp"

#include "neural/activation.hpp"
#include "neural/ann.hpp"
#include "neural/layer.hpp"
#include "tensorlib.hpp"

using ff_network = tensor::neural::ann;

namespace training {

  constexpr static tensor::mat4x2 training_inputs = {
    {
      std::array{ 0.f, 0.f },
      std::array{ 1.f, 0.f },
      std::array{ 0.f, 1.f },
      std::array{ 1.f, 1.f },
    }
  };
  constexpr static tensor::mat4x1 xor_outputs = {
    {
      std::array{ 0.f },
      std::array{ 1.f },
      std::array{ 1.f },
      std::array{ 0.f },
    }
  };

}  // namespace training

int main() {
  srand(time(nullptr));
  tensor::init_tensor();
  {
    tensor::dyn_matrix input = training::training_inputs;
    tensor::dyn_matrix xor_outputs = training::xor_outputs;

    std::array<tensor::natural_t, 3> topology = { 2, 2, 1 };

    auto [rand_weights, rand_biases] = tensor::neural::random_parameters(topology, 0.f, 1.f);
    std::vector<tensor::dyn_matrix> W = rand_weights;
    std::vector<tensor::dyn_vector> b = rand_biases;

    /// simple xor feedforward network with one hidden layer of 2 neurons
    ff_network ann{ { 2, 2, 1 } };
    ann.bind_affine_layer(0, W[0], b[0], tensor::neural::sigmoid_layer);
    ann.bind_affine_layer(1, W[1], b[1], tensor::neural::sigmoid_layer);

    std::print("=========[XOR Model]=========================\n");
    std::print("ANN = {}\n", tensor::as_string(ann));
    std::print("=============================================\n");

    tensor::real_t learning_rate = 0.1f;

    std::println("training model against xor data");
    for (size_t i = 0; i < 50000; ++i) {
      tensor::real_t cost = tensor::neural::compute_cost(input, xor_outputs, ann);
      ff_network gradient = tensor::neural::backpropogate(ann, input, xor_outputs);
      tensor::neural::learn(ann, gradient, learning_rate);

      if (i % 1000 == 0) {
        std::print("=========[cost [{}] = {}]=========================\n", i, cost);
        // std::print("ANN = {}\n", tensor::as_string(ann));
        // std::print("---------------------------------------------\n");
        // std::print("gradient = {}\n", tensor::as_string(gradient));
        // std::print("=============================================\n");
      }
    }

    for (size_t i = 0; i < input.rows; ++i) {
      tensor::dyn_vector in_i = input.get_row(i);
      tensor::dyn_vector o_i = xor_outputs.get_row(i);
      tensor::dyn_vector result = ann.forward(in_i);
      std::print("input: [{}, {}] -> output: [{}] -> expected: [{}]\n", in_i[0], in_i[1], result[0], o_i[0]);
    }
  }

  tensor::shutdown_tensor();
  std::print("exit successful\n");
  return 0;
}