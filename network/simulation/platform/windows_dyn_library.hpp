/**
 * \file simulation/platform/windows_dyn_library.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_PLATFORM_WINDOWS_DYN_LIBRARY_HPP
#define TENSORLIB_NETWORK_SIMULATION_PLATFORM_WINDOWS_DYN_LIBRARY_HPP

#include "simulation/dyn_library.hpp"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace tensor {
  namespace network {

    class windows_dyn_library : public dyn_library {
     public:
      windows_dyn_library(const std::filesystem::path& path)
          : dyn_library(path) {}
      ~windows_dyn_library() override {}

     protected:
      bool load() override;
      void unload() override;
      symbol load_symbol(const std::string_view symname) override;

     private:
      HMODULE module_handle = nullptr;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_PLATFORM_WINDOWS_DYN_LIBRARY_HPP