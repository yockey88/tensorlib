/**
 * \file simulation/simulation_event.cpp
 **/
#include "simulation/simulation_event.hpp"

#include <sstream>

#include <flatbuffers/flexbuffers.h>

namespace tensor {
  namespace network {
    namespace detail {
      namespace {

        static inline void build(message& message, const simulation_event& event) {
          flexbuffers::Builder builder;
          builder.Map([&]() {
            builder.Int("id", event.id);
            builder.Int("time", event.time);
            builder.Vector("node_ids", [&]() {
              for (const auto& node_id : event.node_ids) {
                builder.Int(node_id);
              }
            });
            builder.Int("type", event.type);
          });

          switch (event.type) {
            default:
              break;
          }

          builder.Finish();

          const auto& buffer = builder.GetBuffer();
          std::ranges::copy(builder.GetBuffer().data(), buffer.data() + buffer.size(), std::back_inserter(message.data));
        }

      }  // namespace
    }  // namespace detail

    message simulation_event::get_message() const {
      message msg;
      msg.category = SIMULATION_EVENT;
      msg.type = MSGID_SIM_EVENT;
      detail::build(msg, *this);

      return msg;
    }

    std::string simulation_event::write_string(const simulation_event& event) {
      std::stringstream ss;
      ss << "\nSimulation Event: [\n";
      ss << "  id: " << event.id << "\n";
      ss << "  time: " << event.time << "\n";
      ss << "  node_ids: [\n";
      if (event.node_ids.empty()) {
        ss << "    <empty>\n";
      } else {
        ss << "    ";
        for (natural_t i = 0; i < event.node_ids.size(); ++i) {
          ss << event.node_ids[i];
          if (i < event.node_ids.size() - 1) {
            ss << ", ";
          }
        }
      }
      ss << "  ]\n";
      ss << "  type: " << magic_enum::enum_name(event.type) << "\n";
      ss << "]\n";
      return ss.str();
    }

  }  // namespace network
}  // namespace tensor