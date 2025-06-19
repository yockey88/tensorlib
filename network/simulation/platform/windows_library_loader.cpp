/**
 * \file simulation/platform/windows_library_loader.cpp
 **/
#include <memory>

#include <windows.h>

#include "core/owning_ptr.hpp"

#include "simulation/library_loader.hpp"
#include "simulation/platform/windows_dyn_library.hpp"

namespace tensor {
  namespace network {

    lib_ptr library_loader::load_library(const std::filesystem::path& path) {
      return make_owning_ptr<windows_dyn_library>(path);
    }

    void library_loader::initialize_platform() {
    }

    void library_loader::shutdown_platform() {
    }

  }  // namespace network
}  // namespace tensor