/**
 * \file simulation/simulation_layer.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_LAYER_HPP
#define TENSORLIB_NETWORK_SIMULATION_LAYER_HPP

#include "core/defines.hpp"
#include "core/types.hpp"

#include "simulation/simulation_event.hpp"

namespace tensor {
  namespace network {

    class simulation_state;

    class simulation_state;

    enum simulation_layer_type {
      SIM_EVENT_LAYER = 0,
      SIM_ANALYSIS_LAYER,
      SIM_COMM_LAYER,
    };

    class TENSORCLASS simulation_layer {
     public:
      simulation_layer(simulation_state& sim_state)
          : sim_state(sim_state) {}
      virtual ~simulation_layer() = default;

      virtual void initialize() {}

      virtual void begin_timestep() {}
      virtual void update() {}
      virtual void end_timestep() {}

      virtual void shutdown() {}

      virtual void handle_event(const simulation_event& event) {}

     private:
      simulation_state& sim_state;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_LAYER_HPP