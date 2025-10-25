/**
 * \file neural/driver.cpp
 **/
#include <print>

#include "neural/activation.hpp"
#include "neural/ann.hpp"
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

ff_network create_xor_model() {
  const std::vector<tensor::natural_t> topology = { 2, 2, 1 };
  auto [rand_weights, rand_biases] = tensor::neural::random_parameters(topology, 0.f, 1.f);

  /// using sigmoid activation for hidden layer and relu for output layer seems to work the best for xor
  ff_network model{ topology };
  model.bind_affine_layer(0, rand_weights[0], rand_biases[0], tensor::neural::sigmoid_layer);
  model.bind_affine_layer(1, rand_weights[1], rand_biases[1], tensor::neural::relu_layer);
  return model;
}

int main() {
  srand(time(nullptr));
  tensor::init_tensor();

  {
    tensor::dyn_matrix input = training::training_inputs;
    tensor::dyn_matrix xor_outputs = training::xor_outputs;

    tensor::natural_t iterations = 100000;
    tensor::real_t learning_rate = 0.1f;

    ff_network model = create_xor_model();
    ff_network::train_simple_model(model, input, xor_outputs, iterations, learning_rate);

    for (size_t i = 0; i < input.rows; ++i) {
      tensor::dyn_vector in_i = input.get_row(i);
      tensor::dyn_vector o_i = xor_outputs.get_row(i);
      tensor::dyn_vector result = model.forward(in_i);
      std::print("input: [{}, {}] -> output: [{}] -> expected: [{}]\n", in_i[0], in_i[1], result[0], o_i[0]);
    }
  }

  tensor::shutdown_tensor();
  std::print("exit successful\n");
  return 0;
}