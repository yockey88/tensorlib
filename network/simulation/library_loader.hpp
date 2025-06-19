/**
 * \file simulation/library_loader.hpp
 **/
#ifndef TENSORLIB_NETWORK_LIBRARY_LOADER_HPP
#define TENSORLIB_NETWORK_LIBRARY_LOADER_HPP

#include <concepts>
#include <filesystem>
#include <map>
#include <print>

#include "core/owning_ptr.hpp"

#include "simulation/dyn_library.hpp"

namespace tensor {
  namespace network {

    using lib_ptr = owning_ptr<dyn_library>;

    class library_loader {
     public:
      library_loader() = default;
      ~library_loader() = default;

      static lib_ptr load_library(const std::filesystem::path& path);

      static void initialize_platform();
      static void shutdown_platform();

     private:
      static inline std::vector<uint64_t> hashes;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_LIBRARY_LOADER_HPP