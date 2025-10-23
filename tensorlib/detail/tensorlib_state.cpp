/**
 * \file detail/tensorlib_state.cpp
 **/
#include "detail/tensorlib_state.hpp"

#include <print>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "core/arena.hpp"
#include "core/logger.hpp"

namespace tensor {
  namespace detail {

    static void initialize_memory_arena() {
      std::println("Initializing memory arena");
      memory::allocate_arena(tensorlib().main_arena);
    }

    static void deallocate_memory_arena() {
      std::println("Deallocating memory arena");
      memory::deallocate_arena(tensorlib().main_arena);
    }

    spdlog::sink_ptr stdout_sink_fn() {
#ifdef TENSORLIB_WINDOWS
      return std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
      return std::make_shared<spdlog::sinks::stdout_sink_mt>();
#endif
    }

    spdlog::sink_ptr core_log_file() {
      return std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/tensorlib-core.log", true);
    }

    static void initialize_logger() {
      logger* log = tensorlib().main_logger;
      log->create_logger(logger::kCoreLoggerName, spdlog::level::trace);

      log_sink sink = {
        "console-sink",
        "%^[%l]%$ %v",
        spdlog::level::info,
        stdout_sink_fn,
      };
      log_sink file_sink = {
        "file-sink",
        "[TENSORLIB] @ (%a %b %d %Y %H:%M:%S %z :: thread %t) %^[%l]%$ %v",
        spdlog::level::trace,
        core_log_file,
      };
      std::string_view loggers[] = { logger::kCoreLoggerName };

      log->register_sink(loggers, sink);
      log->register_sink(loggers, file_sink);
      log->send_log(spdlog::level::info, logger::kCoreLoggerName, "Initialized tensorlib logger.");
    }

    static void shutdown_logger() {
      logger* log = tensorlib().main_logger;
      if (log) {
        log->send_log(spdlog::level::info, logger::kCoreLoggerName, "Shutting down tensorlib logger.");
        log->shutdown();
        subsystem<logger>::shutdown();
      } else {
        std::println("Logger subsystem is null. Cannot shutdown logger.");
      }
    }

  }  // namespace detail

  namespace {

    struct state_storage {
      alignas(tensorlib_state) uint8_t storage[sizeof(tensorlib_state)];
      tensorlib_state* ptr() {
        return std::launder(reinterpret_cast<tensorlib_state*>(storage));
      }
      void* address() { return storage; }
    };
    static state_storage tensorlib_global_state;

  }  // namespace

  tensorlib_state& tensorlib() {
    return *tensorlib_global_state.ptr();
  }

  void init_tensor() {
    new (tensorlib_global_state.address()) tensorlib_state();

    tensorlib().main_logger = subsystem<logger>::get();
    tensorlib().main_arena = subsystem<memory::arena>::get();
    TENSORLIB_ASSERT(tensorlib().main_arena != nullptr, "Main arena is null. Memory subsystem not initialized.");

    detail::initialize_memory_arena();
    detail::initialize_logger();
  }

  void shutdown_tensor() {
    detail::shutdown_logger();
    detail::deallocate_memory_arena();

    tensorlib().main_arena = nullptr;
    subsystem<memory::arena>::shutdown();

    std::destroy_at(tensorlib_global_state.address());
    tensorlib_global_state = state_storage();
  }

}  // namespace tensor