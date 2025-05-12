/**
 * \file simulation/dyn_libraries.hpp
 **/
#ifndef TENSORLIB_NETWORK_SIMULATION_DYN_LIBRARY_HPP
#define TENSORLIB_NETWORK_SIMULATION_DYN_LIBRARY_HPP

#include <filesystem>
#include <map>
#include <string>

namespace tensor {
  namespace network {

    struct symbol {
      symbol()
          : name("<blank-symbol>"), address(nullptr) {}
      symbol(const std::string_view name, void* address)
          : name(name), address(address) {}

      std::string name;

      operator bool() const {
        return address != nullptr;
      }

      template <typename Fn>
      Fn get_as_callable() const {
        return reinterpret_cast<Fn>(address);
      }

     private:
      void* address;
    };

    class dyn_library {
     public:
      dyn_library(const std::filesystem::path& path)
          : module_path(path) {}
      virtual ~dyn_library();

      void attempt_load();
      void unload_library();

      bool has_symbol(const std::string_view symname);
      const symbol& get_symbol(const std::string_view symname);

      bool is_loaded() const;
      std::filesystem::path get_path() const;

     protected:
      virtual bool load() = 0;
      virtual void unload() = 0;
      virtual symbol load_symbol(const std::string_view syname) = 0;

     private:
      bool loaded = false;
      std::filesystem::path module_path;

      std::map<uint64_t, symbol> symbols;
    };

  }  // namespace network
}  // namespace tensor

#endif  // TENSORLIB_NETWORK_SIMULATION_DYN_LIBRARY_HPP