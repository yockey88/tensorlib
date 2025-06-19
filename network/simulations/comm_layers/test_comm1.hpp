/**
 * \file simulations/comm_layers/test_comm1.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP
#define TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP

#include "simulation/simulation_layer.hpp"

class TENSORCLASS test_comm1 : public tensor::network::simulation_layer {
 public:
  SIM_LAYER(test_comm1, SIM_COMM_LAYER);

  tensor::network::simulation_layer_type get_layer_type() override {
    return tensor::network::SIM_COMM_LAYER;
  }

  void initialize() override;
  void begin_timestep() override;
  void update() override;
  void end_timestep() override;
  void on_shutdown() override;
};

TENSOR_SIM_LAYER(test_comm1);

#endif  // TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP