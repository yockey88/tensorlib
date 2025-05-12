/**
 * \file simulation/platform/windows_dyn_library.cpp
 **/
#include "simulation/platform/windows_dyn_library.hpp"

#include <iostream>
#include <print>

#include "core/defines.hpp"

namespace tensor {
  namespace network {

    bool windows_dyn_library::load() {
      std::wstring wmod_path = get_path().wstring();
      module_handle = LoadLibraryW(wmod_path.c_str());
      if (module_handle == nullptr) {
        std::print(std::cerr, "Failed to load module: {}", get_path().string());
        return false;
      }

      return true;
    }

    void windows_dyn_library::unload() {
      if (is_loaded()) {
        TENSORLIB_ASSERT(module_handle != nullptr, "Module handle is null");
        if (FreeLibrary(module_handle) == 0) {
          std::print(std::cerr, "Failed to unload module: {} [{}]", get_path().string(), GetLastError());
        }
      }
    }

    symbol windows_dyn_library::load_symbol(const std::string_view syname) {
      FARPROC address = GetProcAddress(module_handle, syname.data());
      if (address == nullptr) {
        std::print(std::cerr, "Failed to load symbol: {} [{}]\n", syname, GetLastError());
        return symbol{ syname.data(), nullptr };
      }

      return symbol{ syname.data(), (void*)address };
    }

  }  // namespace network
}  // namespace tensor