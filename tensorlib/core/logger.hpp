/**
 * \file core/logger.hpp
 **/
#ifndef TENSORLIB_CORE_LOGGER_HPP
#define TENSORLIB_CORE_LOGGER_HPP

#include <fstream>
#include <source_location>
#include <span>
#include <stacktrace>

#include <spdlog/spdlog.h>

#include "core/subsystem.hpp"
#include "core/types.hpp"

namespace tensor {

  using sink_fn = std::function<spdlog::sink_ptr()>;
  struct log_sink {
    std::string sink_name;
    std::string sink_pattern;
    spdlog::level::level_enum level;
    sink_fn sink_factory = nullptr;
  };

  class logger : public subsystem<logger> {
   public:
    logger() = default;
    ~logger() = default;

    void shutdown();

    void create_logger(const std::string_view name, spdlog::level::level_enum level);
    std::optional<uint16_t> register_sink(const std::span<const std::string_view> logs, const log_sink& sink);

    void send_log(spdlog::level::level_enum level, const std::string_view log_name, const std::string_view msg, std::source_location loc = std::source_location::current());
    void send_log(spdlog::level::level_enum level, uint16_t sink_id, const std::string_view msg, std::source_location loc = std::source_location::current());
    void send_log(spdlog::level::level_enum level, const std::vector<uint16_t>& sink_id, const std::string_view msg, std::source_location loc = std::source_location::current());

    constexpr static std::string_view kCoreLoggerName = "tensorlib-core-log";

   private:
    struct sink_data {
      spdlog::sink_ptr sink;
      std::string_view main_logger_name;
      sink_data(spdlog::sink_ptr s, std::string_view logger_name)
          : sink(std::move(s)), main_logger_name(logger_name) {}
    };

    constexpr static std::string_view kFallbackFile = "logs/tensorlib.log";
    constexpr static std::string_view kLogFailureFile = "logs/tensorlib-log-failure.log";
    std::ofstream error_log_file;

    std::map<uint16_t, sink_data> sinks;
    std::map<natural_t, cref<spdlog::logger>> loggers;

    void log_failure_error(const std::string& message);

    uint16_t next_sink_id = 1;
    static constexpr uint16_t kMaxSinkId = 0xFFFF;  // Maximum value for a 16-bit unsigned integer
    uint16_t get_next_sink_id() {
      next_sink_id = next_sink_id == kMaxSinkId ? 1 : next_sink_id + 1;
      return next_sink_id;
    }
  };

}  // namespace tensor

TENSORLIB_SUBSYSTEM(tensor::logger);

#define LOG(level, logger_name, frmt, ...) \
  tensor::subsystem<tensor::logger>::get()->send_log(level, logger_name, std::format(frmt, ##__VA_ARGS__))

#define LOG_TO_SINK(sink_id, frmt, ...) \
  tensor::subsystem<tensor::logger>::get()->send_log(spdlog::level::info, sink_id, std::format(frmt, ##__VA_ARGS__))

#define ___LOG_TRACE(logger_name, fmt, ...) LOG(spdlog::level::trace, logger_name, fmt, __VA_ARGS__)
#define ___LOG_DEBUG(logger_name, fmt, ...) LOG(spdlog::level::debug, logger_name, fmt, __VA_ARGS__)
#define ___LOG_INFO(logger_name, fmt, ...) LOG(spdlog::level::info, logger_name, fmt, __VA_ARGS__)
#define ___LOG_WARN(logger_name, fmt, ...) LOG(spdlog::level::warn, logger_name, fmt, __VA_ARGS__)
#define ___LOG_ERROR(logger_name, fmt, ...) LOG(spdlog::level::err, logger_name, fmt, __VA_ARGS__)
#define ___LOG_CRITICAL(logger_name, fmt, ...) LOG(spdlog::level::critical, logger_name, fmt, __VA_ARGS__)

#define ___LOG_TRACE_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)
#define ___LOG_DEBUG_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)
#define ___LOG_INFO_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)
#define ___LOG_WARN_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)
#define ___LOG_ERROR_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)
#define ___LOG_CRITICAL_TO_SINK(sink_id, fmt, ...) LOG_TO_SINK(sink_id, fmt, __VA_ARGS__)

/// core logger macros
#define CORE_LOG_TRACE(format, ...) ___LOG_TRACE(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)
#define CORE_LOG_DEBUG(format, ...) ___LOG_DEBUG(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)
#define CORE_LOG_INFO(format, ...) ___LOG_INFO(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)
#define CORE_LOG_WARN(format, ...) ___LOG_WARN(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)
#define CORE_LOG_ERROR(format, ...) ___LOG_ERROR(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)
#define CORE_LOG_CRITICAL(format, ...) ___LOG_CRITICAL(tensor::logger::kCoreLoggerName, format, __VA_ARGS__)

/// specific sink logging macros
#define LOG_TRACE(sink_id, format, ...) ___LOG_TRACE_TO_SINK(sink_id, format, __VA_ARGS__)
#define LOG_DEBUG(sink_id, format, ...) ___LOG_DEBUG_TO_SINK(sink_id, format, __VA_ARGS__)
#define LOG_INFO(sink_id, format, ...) ___LOG_INFO_TO_SINK(sink_id, format, __VA_ARGS__)
#define LOG_WARN(sink_id, format, ...) ___LOG_WARN_TO_SINK(sink_id, format, __VA_ARGS__)
#define LOG_ERROR(sink_id, format, ...) ___LOG_ERROR_TO_SINK(sink_id, format, __VA_ARGS__)
#define LOG_CRITICAL(sink_id, format, ...) ___LOG_CRITICAL_TO_SINK(sink_id, format, __VA_ARGS__)

/// \todo add automatic enter/exit function logger structs (raii tracing)

#if __has_include(<stacktrace>)
  #define GET_STACKTRACE (std::stringstream{} << std::stacktrace::current() << "\n").str()
#else
  #define GET_STACKTRACE "Stacktrace not available (std::stacktrace not included)"
#endif

#ifndef TENSOR_TEST_ENVIRONMENT
  #define TENSOR_ABORT std::terminate()
#else
  #define TENSOR_ABORT throw std::runtime_error("Critical failure in test environment, aborting.");
#endif

#define TENSOR_CRITICAL_FAILURE(format, ...)                                                          \
  do {                                                                                                \
    CORE_LOG_CRITICAL("Critical failure!\nstacktrace =\n{}\n" format, GET_STACKTRACE, ##__VA_ARGS__); \
    TENSOR_ABORT;                                                                                     \
  } while (0)

#define TENSORLIB_ASSERT(condition, format, ...)    \
  do {                                              \
    if ((condition)) {                              \
    } else {                                        \
      TENSOR_CRITICAL_FAILURE(format, __VA_ARGS__); \
    }                                               \
  } while (0)

#endif  // TENSORLIB_CORE_LOGGER_HPP