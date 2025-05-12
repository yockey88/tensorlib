/**
 * \file /neural/ann.cpp
 **/
#include "neural/ann.hpp"

#include <algorithm>
#include <print>
#include <sstream>

#include "core/serialization.hpp"
#include "core/types.hpp"

#include "activation.hpp"
#include "linalgebra/dyn_matrix.hpp"
#include "linalgebra/dyn_vector.hpp"

namespace tensor {
  namespace neural {

    ann::ann(std::initializer_list<natural_t> topology)
        : L(topology.size()) {
      TENSORLIB_ASSERT(L > 0, "Topology must have at least one layer.");

      std::vector<natural_t> topology_buffer(topology.size());
      std::ranges::copy(topology, topology_buffer.begin());

      initialize(topology_buffer);
    }

    ann::ann(const std::span<natural_t> topology)
        : L(topology.size()) {
      TENSORLIB_ASSERT(L > 0, "Topology must have at least one layer.");
      initialize(topology);
    }

    ann::ann(const std::span<natural_t> topology, const std::vector<dyn_matrix>& W, const std::vector<dyn_vector>& b)
        : L(topology.size()), W(W), b(b) {
      TENSORLIB_ASSERT(L > 0, "Topology must have at least one layer.");
      initialize(topology, false);
    }

    ann::ann(const std::span<natural_t> topology, const std::vector<dyn_matrix>& W, const std::vector<dyn_vector>& b, const std::vector<layer_activation>& activation_functions)
        : L(topology.size()), W(W), b(b), activation_functions(activation_functions) {
      TENSORLIB_ASSERT(L > 0, "Topology must have at least one layer.");
      initialize(topology, false, false);
    }

    natural_t ann::input_size() const {
      return l_i[0];
    }

    natural_t ann::output_size() const {
      return l_i[L - 1];
    }

    natural_t ann::num_layers() const {
      return L;
    }

    natural_t ann::num_hidden_layers() const {
      return L - 1;
    }

    dyn_matrix& ann::weight(natural_t layer_index) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      return W[layer_index];
    }

    dyn_vector& ann::bias(natural_t layer_index) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      return b[layer_index];
    }

    dyn_vector& ann::output(natural_t layer_index) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      return layer_outputs[layer_index];
    }

    dyn_vector& ann::activated_output(natural_t layer_index) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      return activated_outputs[layer_index];
    }

    layer_activation& ann::activation(natural_t layer_index) {
      return activation_functions[layer_index];
    }

    dyn_vector ann::forward(const dyn_vector& input) {
      TENSORLIB_ASSERT(input.size > 0, "Input vector is empty.");
      TENSORLIB_ASSERT(input.size == this->l_i[0], "Input size does not match ANN input size.");

      layer_outputs.clear();
      layer_outputs.resize(L);
      for (size_t i = 0; i < L; ++i) {
        layer_outputs[i] = dyn_vector(l_i[i]);
      }

      activated_outputs.clear();
      activated_outputs.resize(L);
      for (size_t i = 0; i < L; ++i) {
        activated_outputs[i] = dyn_vector(l_i[i]);
      }

      layer_outputs[0] = input;
      layer_bound[0] = YesNo::Yes;
      on_validate();

      activated_outputs[0] = layer_outputs[0];
      for (size_t i = 0; i < L - 1; ++i) {
        TENSORLIB_ASSERT(activation_functions[i].activation_function != nullptr, "Activation function is not set.");
        TENSORLIB_ASSERT(W[i].rows == l_i[i + 1] && W[i].cols == l_i[i], "Matrix dimensions do not match.");
        TENSORLIB_ASSERT(b[i].size == l_i[i + 1], "Bias vector size does not match layer size.");

        dyn_vector product = dyn_matrix_vector_product(W[i], activated_outputs[i]);
        dyn_vector sum = dyn_vector_sum(product, b[i]);
        layer_outputs[i + 1] = sum;
        activated_outputs[i + 1] = activation_functions[i].activation_function(sum);
      }

      return activated_outputs.back();
    }

    void ann::zero() {
      for (natural_t l = 0; l < L; ++l) {
        for (natural_t i = 0; i < layer_outputs[l].size; ++i) {
          layer_outputs[l][i] = 0.f;
        }
      }

      for (natural_t l = 0; l < L - 1; ++l) {
        for (natural_t i = 0; i < W[l].data.size(); ++i) {
          W[l].data[i] = 0.f;
        }
        for (natural_t i = 0; i < b[l].data.size(); ++i) {
          b[l].data[i] = 0.f;
        }
      }
    }

    natural_t ann::get_layer_size(natural_t layer_index) const {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      return l_i[layer_index];
    }

    void ann::bind_affine_layer(natural_t layer_index, const ann& other) {
      bind_affine_layer(layer_index, other.W[layer_index], other.b[layer_index], other.activation_functions[layer_index]);
    }

    void ann::bind_affine_layer(natural_t layer_index, layer_activation activation_function) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      activation_functions[layer_index] = activation_function;

      /// could probably do this better
      try {
        [[maybe_unused]] auto& _ = W.at(layer_index);
        [[maybe_unused]] auto& _b = b.at(layer_index);
        layer_bound[layer_index] = YesNo::Yes;
      } catch (...) {
        layer_bound[layer_index] = YesNo::No;
      }
    }

    void ann::bind_affine_layer(natural_t layer_index, const dyn_matrix& W, const dyn_vector& b, layer_activation activation_function) {
      TENSORLIB_ASSERT(layer_index < L, "Layer index out of bounds.");
      this->W[layer_index] = W;
      this->b[layer_index] = b;
      this->activation_functions[layer_index] = activation_function;

      layer_bound[layer_index] = YesNo::Yes;
    }

    std::string ann::print_topology() const {
      std::stringstream ss;
      ss << "ANN Topology: ";
      ss << " -- " << l_i.size() << " layers\n";
      ss << " -- Input size: " << l_i.front() << "\n";
      ss << " -- Output size: " << l_i.back() << "\n";
      ss << " -- Hidden layers: " << L - 1 << "\n";
      ss << " -- Total Layers = " << L + 1 << "\n";
      ss << " -- layers --\n";
      for (size_t i = 0; i < L; ++i) {
        ss << "[layer [" << i << "], dim = " << l_i[i] << "]";
        ss << ((i == L - 1) ? "(output layer)\n" : "\n");
        ss << "- current output = " << core::as_string(layer_outputs[i]);

        if (i < L - 1) {
          ss << "- parameters:\n";
          ss << "> W = " << core::as_string(W[i]);
          ss << "> b = " << core::as_string(b[i]);
          ss << "]\n";
        }
      }
      return ss.str();
    }

    void ann::initialize(const std::span<natural_t> topology, bool randomize_parameters, bool default_activation) {
      l_i.resize(topology.size());
      std::ranges::copy(topology, l_i.begin());

      layer_outputs.resize(L);
      activated_outputs.resize(L);

      layer_bound.resize(L);
      std::ranges::fill(layer_bound, YesNo::No);

      activation_functions.resize(L - 1);
      if (randomize_parameters) {
        auto [rand_weights, rand_biases] = random_parameters(topology, -1.f, 1.f);
        W = rand_weights;
        b = rand_biases;
      }

      if (default_activation) {
        for (size_t i = 0; i < L - 1; ++i) {
          activation_functions[i] = neural::relu_layer;
        }
      }
    }

    void ann::on_validate() const {
      for (size_t i = 0; i < layer_bound.size() - 1; ++i) {
        if (layer_bound[i] == YesNo::No) {
          std::print("Layer [{}] is not bound to a parameter set.\n", i);
        }
      }
    }

    std::pair<std::vector<dyn_matrix>, std::vector<dyn_vector>> random_parameters(const std::span<natural_t> topology, real_t min, real_t max) {
      std::vector<dyn_matrix> W;
      std::vector<dyn_vector> b;

      for (size_t i = 0; i < topology.size() - 1; ++i) {
        W.push_back(tensor::rand_matrix(topology[i + 1], topology[i], min, max));
        b.push_back(tensor::rand_vector(topology[i + 1], min, max));
      }

      return { W, b };
    }

    real_t compute_cost(const dyn_matrix& input_data, const dyn_matrix& output_data, ann& model) {
      natural_t n = input_data.rows;

      real_t cost = 0.f;
      for (size_t i = 0; i < n; ++i) {
        dyn_vector in_i = input_data.get_row(i);
        dyn_vector o_i = output_data.get_row(i);

        dyn_vector result = model.forward(in_i);

        for (size_t j = 0; j < result.size; ++j) {
          dyn_vector diffv = dyn_vector_difference(result, o_i);
          cost += dyn_vector_dot_product(diffv, diffv);
        }
      }

      return cost / n;
    }

    void finite_difference(real_t cost, ann& model, ann& gradient, const dyn_matrix& input_data, const dyn_matrix& output_data, real_t eps) {
      real_t saved = 0.f;

      for (natural_t l = 0; l < model.num_hidden_layers() - 1; ++l) {
        dyn_matrix& W = model.weight(l);
        dyn_vector& b = model.bias(l);

        dyn_matrix& gW = gradient.weight(l);
        dyn_vector& gb = gradient.bias(l);

        for (natural_t i = 0; i < W.data.size(); ++i) {
          saved = W.data[i];
          W.data[i] += eps;
          gW.data[i] = (compute_cost(input_data, output_data, model) - cost) / eps;
          W.data[i] = saved;
        }

        for (natural_t i = 0; i < b.data.size(); ++i) {
          saved = b.data[i];
          b.data[i] += eps;
          gb.data[i] = (compute_cost(input_data, output_data, model) - cost) / eps;
          b.data[i] = saved;
        }
      }
    }

    void learn(ann& model, ann& gradient, real_t learning_rate) {
      for (natural_t l = 0; l < model.num_hidden_layers() - 1; ++l) {
        for (natural_t i = 0; i < model.weight(l).data.size(); ++i) {
          model.weight(l).data[i] -= learning_rate * gradient.weight(l).data[i];
        }
        for (natural_t i = 0; i < model.bias(l).data.size(); ++i) {
          model.bias(l).data[i] -= learning_rate * gradient.bias(l).data[i];
        }
      }
    }

    ann backpropogate(ann& model, const dyn_matrix& input_data, const dyn_matrix& output_data) {
      TENSORLIB_ASSERT(input_data.rows == output_data.rows, "Input and output data must have the same number of rows.");
      TENSORLIB_ASSERT(input_data.cols == model.input_size(), "Input data size does not match ANN input size.");
      TENSORLIB_ASSERT(output_data.cols == model.output_size(), "Output data size does not match ANN output size.");

      ann gradient = model;
      gradient.zero();

      for (size_t i = 0; i < input_data.rows; ++i) {
        dyn_vector in_i = input_data.get_row(i);
        dyn_vector o_i = output_data.get_row(i);
        dyn_vector result = model.forward(in_i);

        ///   if C(N, x) = sum_1,M(|N(x) - y|^2)
        ///   then, for ech idx
        //          dC_i/d<var> = 2 * (N(x) - y) * dN_i/d<var>
        //     and for each var
        //          dN_i/d<var> = f'(N_i) * dN_i/d<var>
        //    so, we have dc = 2 * (N(x) - y) * f'(N_i)
        ///   then dC_i/d<var> = 2 * (N(x) - y) * f'(N_i) * dN_i/d<var>
        ///        and dC_i / d<var> = dc * dN_i/d<var>

        ///   where dc = 2 * (N(x) - y)
        dyn_vector dc = dyn_vector_difference(result, o_i);
        dc = dyn_vector_scalar_product(dc, 2.f);

        gradient.output(model.num_hidden_layers()) = dc;

        for (natural_t layer = model.num_hidden_layers(); layer > 0; --layer) {
          // /// use layer + 1 because we want the size of the vector this layer outputs to iterate over
          // /// here we use layer for both activation/output because this is the f'(W_l * x_(l-1) + b_l) part of the gradient
          dyn_vector activation_derivative = model.activation(layer - 1).derivative_function(model.output(layer));
          dyn_vector gradient_output = gradient.output(layer);

          for (natural_t r = 0; r < model.output(layer).size; ++r) {
            real_t grad_output = gradient_output[r];
            real_t act_derivative = activation_derivative[r];

            gradient.bias(layer - 1)[r] += grad_output * act_derivative;
            for (size_t c = 0; c < model.output(layer - 1).size; ++c) {
              real_t weight_shift = model.output(layer - 1)[c];
              real_t output_shift = model.weight(layer - 1)(r, c);

              gradient.weight(layer - 1)(r, c) += grad_output * act_derivative * weight_shift;
              gradient.output(layer - 1)[c] += grad_output * act_derivative * output_shift;
            }
          }
        }
      }

      for (natural_t layer = 0; layer < model.num_hidden_layers(); ++layer) {
        for (natural_t i = 0; i < gradient.weight(layer).data.size(); ++i) {
          gradient.weight(layer)[i] /= input_data.rows;
        }
        for (natural_t i = 0; i < gradient.bias(layer).data.size(); ++i) {
          gradient.bias(layer)[i] /= input_data.rows;
        }
      }

      return gradient;
    }

  }  // namespace neural
}  // namespace tensor