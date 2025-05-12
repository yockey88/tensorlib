/**
 * \file message/message.hpp
 **/
#ifndef TENSORLIB_NETWORK_MESSAGE_HPP
#define TENSORLIB_NETWORK_MESSAGE_HPP

#include <cstdint>
#include <format>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "core/types.hpp"

#include "message/channel.hpp"

namespace tensor {
  namespace network {

    enum message_category : uint16_t {
      NOTIFICATION = 0,

      CONTROL = bit(0),

      COMMAND = bit(1),
      QUERY = bit(2),
      RESPONSE = bit(3),

      ACKNOWLEDGEMENT = bit(4),
      ERROR_ALERT = bit(5),

      INFO = bit(6),

      SIMULATION = bit(7),

      SIMULATION_EVENT = bit(8),
    };

    enum message_id : uint16_t {
      MSGID_ACK = bit(1),
      MSGID_NACK = bit(2),

      MSGID_THREAD_INITIALIZE = bit(3),
      MSGID_THREAD_START = bit(4),
      MSGID_THREAD_SHUTDOWN = bit(5),

      MSGID_SIM_EVENT = bit(6),
    };

    static inline uint32_t get_message_header(message_category category, message_id id) {
      return (static_cast<uint32_t>(category) << 16) | static_cast<uint32_t>(id);
    }

    static inline message_category msg_category(uint32_t header) {
      return static_cast<message_category>(header >> 16);
    }

    static inline message_id msg_id(uint32_t header) {
      return static_cast<message_id>(header & 0xFFFF);
    }

    struct message {
      message_category category;
      message_id type;

      /// fix this, don't want to pass vector around, how to pass safe buffers between thread to avoid copy?
      std::vector<uint8_t> data;

      message() = default;
      template <typename... Args>
      message(message_category category, message_id type, Args&&... args)
          : category(category), type(type) {
        // data.reserve(sizeof...(args));
        // data.insert(data.end(), { static_cast<uint8_t>(args)... });
      }
    };

    using message_channel = channel<message>;

  }  // namespace network
}  // namespace tensor

template <typename E>
  requires std::is_enum_v<E>
struct std::formatter<E> : public std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(E e, FormatContext& ctx) const {
    return std::formatter<std::string_view>::format(magic_enum::enum_name(e), ctx);
  }
};

#endif  // TENSORLIB_NETWORK_MESSAGE_HPP