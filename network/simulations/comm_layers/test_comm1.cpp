/**
 * \file simulations/comm_layers/test_comm1.cpp
 **/
#include "simulations/comm_layers/test_comm1.hpp"

#include <print>

void test_comm1::initialize() {
  std::println("test_comm1::initialize");
}

void test_comm1::begin_timestep() {
}

void test_comm1::update() {
}

void test_comm1::end_timestep() {
}

void test_comm1::on_shutdown() {
  std::println("test_comm1::on_shutdown");
}
