/**
 * \file simulation/read_write_buffer.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_READ_WRITE_BUFFER_HPP
#define TENSORLIB_NETWORK_SIMULATION_READ_WRITE_BUFFER_HPP

#include <array>
#include <queue>

namespace tensor {
  namespace network {

    struct read_write_buffer {
      constexpr static size_t kBufferSize = 2048;

      bool reading = false;
      std::array<uint8_t, kBufferSize> read_buffer;

      bool writing = false;
      std::array<uint8_t, kBufferSize> write_buffer;
      std::queue<std::vector<uint8_t>> write_queue;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_READ_WRITE_BUFFER_HPP