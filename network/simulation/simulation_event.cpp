/**
 * \file simulation/simulation_event.cpp
 **/
#include "simulation/simulation_event.hpp"

#include <sstream>

#include <flatbuffers/flexbuffers.h>

namespace tensor {
  namespace network {

    event_time& event_time::operator=(const event_time& other) {
      min_step = other.min_step;
      max_step = other.max_step;
      return *this;
    }

    message simulation_event::get_message() const {
      message msg(SIMULATION_EVENT, SIM_EVENT);
      flexbuffers::Builder builder;
      builder.Map([&]() {
        builder.Int("id", id);
        // builder.Int("time", time);
        builder.Vector("node_ids", [&]() {
          for (const auto& node_id : node_ids) {
            builder.Int(node_id);
          }
        });
        builder.Int("type", type);
      });

      switch (type) {
        default:
          break;
      }

      builder.Finish();

      const auto& buffer = builder.GetBuffer();
      std::ranges::copy(builder.GetBuffer().data(), buffer.data() + buffer.size(), std::back_inserter(msg.data));

      return msg;
    }

    std::string simulation_event::write_string(const simulation_event& event) {
      std::stringstream ss;
      ss << "\nSimulation Event: [\n";
      ss << "  id: " << event.id << "\n";
      ss << "  time: { min = " << event.time.min_step << " , max = " << event.time.max_step << " }\n";
      ss << "  node_ids: [\n";
      if (event.node_ids.empty()) {
        ss << "    <all-nodes>\n";
      } else {
        ss << "    ";
        for (natural_t i = 0; i < event.node_ids.size(); ++i) {
          ss << event.node_ids[i];
          if (i < event.node_ids.size() - 1) {
            ss << ", ";
          }
        }
        ss << "\n";
      }
      ss << "  ]\n";
      ss << std::format("  type: {}\n", magic_enum::enum_name(event.type));
      ss << "]\n";
      return ss.str();
    }

  }  // namespace network
}  // namespace tensor