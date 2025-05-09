/**
 * \file neural/ann.hpp
 **/
#ifndef TENSORLIB_NEURAL_ANN_HPP
#define TENSORLIB_NEURAL_ANN_HPP

#include <initializer_list>
#include <span>
#include <string>

#include "core/buffer.hpp"
#include "core/types.hpp"

#include "linalgebra/dyn_matrix.hpp"
#include "linalgebra/dyn_vector.hpp"
#include "neural/activation.hpp"

namespace tensor {
  namespace neural {

    struct ann {
      ann(std::initializer_list<natural_t> topology);
      ann(const std::span<natural_t> topology);

      ann(const std::span<natural_t> topology, const std::vector<dyn_matrix>& W, const std::vector<dyn_vector>& b, const std::vector<layer_activation>& activation_functions);

      natural_t input_size() const;
      natural_t output_size() const;

      natural_t num_layers() const;
      natural_t num_hidden_layers() const;

      const std::vector<natural_t>& get_topology() const {
        return l_i;
      }

      dyn_matrix& weight(natural_t layer_index);
      dyn_vector& bias(natural_t layer_index);
      dyn_vector& output(natural_t layer_index);
      layer_activation& activation(natural_t layer_index);

      dyn_vector forward(const dyn_vector& input);
      void zero();

      natural_t get_layer_size(natural_t layer_index) const;
      void bind_affine_layer(natural_t layer_index, const ann& other);
      void bind_affine_layer(natural_t layer_index, layer_activation activation_function);
      void bind_affine_layer(natural_t layer_index, const dyn_matrix& W, const dyn_vector& b, layer_activation activation_function);

      std::string print_topology() const;

      static std::string write_string(const ann& model) {
        return model.print_topology();
      }

      const std::vector<dyn_vector>& get_layer_outputs() const {
        return layer_outputs;
      }

     private:
      enum YesNo {
        No = 0,
        Yes = 1
      };
      std::vector<YesNo> layer_bound = { No };

      natural_t L = 0;
      std::vector<natural_t> l_i;

      std::vector<dyn_matrix> W;
      std::vector<dyn_vector> b;

      std::vector<layer_activation> activation_functions;

      std::vector<dyn_vector> layer_outputs;

      void initialize(const std::span<natural_t> topology, bool randomize_parameters = true);
      void on_validate() const;
    };

    std::pair<std::vector<dyn_matrix>, std::vector<dyn_vector>> random_parameters(const std::span<natural_t> topology, real_t min = -1.f, real_t max = 1.f);

    real_t compute_cost(const dyn_matrix& input_data, const dyn_matrix& output_data, ann& model);
    void finite_difference(real_t cost, ann& model, ann& gradient, const dyn_matrix& input_data, const dyn_matrix& output_data, real_t eps = 1e-1f);

    ann backpropogate(ann& model, const dyn_matrix& input_data, const dyn_matrix& output_data);

    void learn(ann& model, ann& gradient, real_t learning_rate);

  }  // namespace neural
}  // namespace tensor

#endif  // TENSORLIB_NEURAL_ANN_HPP