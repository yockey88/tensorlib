/**
 * \file simulations/comm_layers/test_comm1.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP
#define TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP

#include "simulation/simulation_layer.hpp"
#include "tensorlib.hpp"

class TENSORCLASS test_comm1 : public tensor::network::simulation_layer {
 public:
  test_comm1(tensor::network::simulation_state& sim_state)
      : simulation_layer(sim_state) {}

  void initialize() override;
  void begin_timestep() override;
  void update() override;
  void end_timestep() override;
  void shutdown() override;
};

extern "C" {
TENSORAPI tensor::network::simulation_layer* create_layer(tensor::network::simulation_state& sim_state);
TENSORAPI void delete_layer(tensor::network::simulation_layer* layer);
}

#endif  // TENSORLIB_NETWORK_SIMULATIONS_COMM_LAYERS_TEST_COMM1_HPP