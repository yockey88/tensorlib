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

  // clang-format off
  ff_network train_simple_model(const std::vector<tensor::natural_t>& topology, tensor::dyn_matrix& input_data, tensor::dyn_matrix& output_data, 
                                const tensor::natural_t iterations, const tensor::real_t learning_rate) {
    // clang-format on
    auto [rand_weights, rand_biases] = tensor::neural::random_parameters(topology, 0.f, 1.f);
    std::vector<tensor::dyn_matrix> W = rand_weights;
    std::vector<tensor::dyn_vector> b = rand_biases;

    /// simple xor feedforward network with one hidden layer of 2 neurons
    ff_network model{ topology };
    model.bind_affine_layer(0, W[0], b[0], tensor::neural::sigmoid_layer);
    model.bind_affine_layer(1, W[1], b[1], tensor::neural::sigmoid_layer);
    /// we have to compute an initial cost as a sort of 'warmup' for the system (this needs to be fixed later)
    ///   this is a hack because the model is 'lazy' in an extremely loose sense
    //    (pls I know lazy doesnt apply here, but like it does even if it doesnt, you know? like it is but it isnt?
    //     like i swear it is i promise like actually tho)
    tensor::real_t initial_cost = tensor::neural::compute_cost(input_data, output_data, model);
    std::println("training model against xor data, initial cost: {}", initial_cost);
    for (size_t i = 0; i < iterations; ++i) {
      ff_network gradient = tensor::neural::backpropogate(model, input_data, output_data);
      tensor::neural::learn(model, gradient, learning_rate);
    }
    tensor::real_t final_cost = tensor::neural::compute_cost(input_data, output_data, model);
    std::println("training model against xor data, final cost: {}", final_cost);

    return model;
  }

}  // namespace training

int main() {
  srand(time(nullptr));
  tensor::init_tensor();
  {
    tensor::dyn_matrix input = training::training_inputs;
    tensor::dyn_matrix xor_outputs = training::xor_outputs;

    tensor::natural_t iterations = 100000;
    tensor::real_t learning_rate = 0.1f;
    ff_network model = training::train_simple_model({ 2, 2, 1 }, input, xor_outputs, iterations, learning_rate);

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