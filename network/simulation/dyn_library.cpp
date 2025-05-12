/**
 * \file simulation/dyn_library.cpp
 **/
#include "simulation/dyn_library.hpp"

#include <iostream>
#include <print>

#include "core/defines.hpp"

namespace tensor {
  namespace network {

    dyn_library::~dyn_library() {
      unload_library();
    }

    void dyn_library::attempt_load() {
      if (loaded) {
        return;
      }

      loaded = load();
      if (!loaded) {
        std::print(std::cerr, "Failed to load library: {}", module_path.string());
      }
    }

    void dyn_library::unload_library() {
      if (!loaded) {
        return;
      }

      unload();
      loaded = false;
    }

    bool dyn_library::has_symbol(const std::string_view symname) {
      if (!loaded) {
        std::print(std::cerr, "Library not loaded: {}", module_path.string());
        return false;
      }

      if (symbols.find(FNV(symname)) != symbols.end()) {
        return true;
      }

      symbol sym = load_symbol(symname);
      if (sym.name.empty()) {
        return false;
      }

      symbols[FNV(symname)] = sym;
      return true;
    }

    const symbol& dyn_library::get_symbol(const std::string_view symname) {
      TENSORLIB_ASSERT(has_symbol(symname), std::format("Symbol not found: {}", symname).c_str());
      return symbols.at(FNV(symname));
    }

    bool dyn_library::is_loaded() const {
      return loaded;
    }

    std::filesystem::path dyn_library::get_path() const {
      return module_path;
    }

  }  // namespace network
}  // namespace tensor