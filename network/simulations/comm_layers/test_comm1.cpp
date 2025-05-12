/**
 * \file simulations/comm_layers/test_comm1.cpp
 **/
#include "simulations/comm_layers/test_comm1.hpp"

#include <print>

void test_comm1::initialize() {
  std::println("test_comm1 layer initialized");
}

void test_comm1::begin_timestep() {
  std::println("test_comm1 layer begin simulation timestep");
}

void test_comm1::update() {
  std::println("test_comm1 layer update");
}

void test_comm1::end_timestep() {
  std::println("test_comm1 layer finalize simulation timestep");
}

void test_comm1::shutdown() {
  std::println("test_comm1 layer finalized");
}

extern "C" {
TENSORAPI tensor::network::simulation_layer* create_layer(tensor::network::simulation_state& sim_state) {
  return new test_comm1(sim_state);
}

TENSORAPI void delete_layer(tensor::network::simulation_layer* layer) {
  delete layer;
}
}