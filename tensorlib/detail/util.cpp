/**
 * \file detail/util.cpp
 **/
#include "detail/util.hpp"

#include <fstream>
#include <iostream>
#include <print>

#include "core/logger.hpp"

namespace tensor {
  namespace detail {

    std::vector<uint8_t> read_to_bytes(const std::string& filename) {
      std::ifstream file(filename, std::ios::binary | std::ios::in);
      if (!file.is_open()) {
        CORE_LOG_ERROR("Failed to open file: {}", filename);
        return {};
      }

      file.seekg(0, std::ios::end);
      size_t size = file.tellg();
      file.seekg(0, std::ios::beg);

      std::vector<uint8_t> buffer(size);
      file.read(reinterpret_cast<char*>(buffer.data()), size);
      file.close();
      return buffer;
    }

  }  // namespace detail
}  // namespace tensor