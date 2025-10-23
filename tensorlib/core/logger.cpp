/**
 * \file core/logger.cpp
 **/
#include "core/logger.hpp"

namespace tensor {

  void logger::shutdown() {
    // PROFILE_SECTION("Logger--Shutdown");

    for (auto& [id, sink] : sinks) {
      if (sink.sink) {
        sink.sink->flush();
      }
    }

    for (auto& [id, logger] : loggers) {
      if (logger) {
        logger->flush();
      }
    }

    loggers.clear();
    sinks.clear();

    next_sink_id = 1;

    if (error_log_file.is_open()) {
      error_log_file.close();
    }
  }

  void logger::create_logger(const std::string_view name, spdlog::level::level_enum level) {
    // PROFILE_SECTION("Logger--CreateLogger");
    std::unique_ptr<spdlog::logger> logger = std::make_unique<spdlog::logger>(std::string{ name });
    logger->set_level(level);
    logger->flush_on(level);

    natural_t id = FNV(name);
    auto [itr, inserted] = loggers.insert({ id, std::move(logger) });
    if (!inserted) {
      log_failure_error(std::format("Logger with name {} already exists.", name));
      logger = nullptr;
    }
  }

  std::optional<uint16_t> logger::register_sink(const std::span<const std::string_view> logs, const log_sink& sink) {
    // PROFILE_SECTION("Logger--RegisterSink");

    if (logs.empty()) {
      log_failure_error("No logs provided for sink registration.");
      return {};
    }

    if (sink.sink_factory == nullptr) {
      log_failure_error(std::format("Sink factory for {} is null.", sink.sink_name));
      return {};
    }

    if (sink.sink_name.empty()) {
      log_failure_error(std::format("Sink name for {} is empty.", sink.sink_name));
      return {};
    }

    if (sink.sink_pattern.empty()) {
      log_failure_error(std::format("Sink pattern for {} is empty.", sink.sink_name));
      return {};
    }

    log_sink sink_copy = sink;

    auto sink_ptr = sink.sink_factory();
    if (sink_ptr == nullptr) {
      log_failure_error(std::format("Failed to create sink for {}.", sink.sink_name));
      return {};
    }

    uint16_t id = get_next_sink_id();
    auto [itr, inserted] = sinks.insert({ id, sink_data(std::move(sink_ptr), logs[0]) });
    if (!inserted) {
      log_failure_error(std::format("Sink with ID {} ({}) already exists.", id, sink.sink_name));
      return {};
    }

    {
      auto& sink_ptr = itr->second.sink;
      sink_ptr->set_pattern(sink.sink_pattern);
      sink_ptr->set_level(sink.level);
      for (const auto& log : logs) {
        auto logger_itr = loggers.find(FNV(log));

        if (logger_itr != loggers.end()) {
          logger_itr->second->sinks().push_back(sink_ptr);
        } else {
          log_failure_error(std::format("Logger {} not found for sink {}.", log, sink.sink_name));
        }
      }
    }

    return id;
  }

  void logger::send_log(spdlog::level::level_enum level, const std::string_view log_name, const std::string_view msg, std::source_location loc) {
    auto logger_itr = loggers.find(FNV(log_name));
    if (logger_itr == loggers.end()) {
      log_failure_error(std::format("Logger {} not found. Dropped Log :\n{}", log_name, msg));
      return;
    }

    auto& logger = logger_itr->second;
    if (logger == nullptr) {
      log_failure_error(std::format("Logger {} is null. Dropped Log :\n{}", log_name, msg));
      return;
    }

    if (logger->level() == spdlog::level::off) {
      return;
    }

    std::string formmatted_msg = std::format("[{}] {}: {}", loc.file_name(), loc.line(), msg);
    switch (level) {
      case spdlog::level::trace:
        logger->trace(msg);
        break;
      case spdlog::level::debug:
        logger->debug(msg);
        break;
      case spdlog::level::info:
        logger->info(msg);
        break;
      case spdlog::level::warn:
        logger->warn(msg);
        break;
      case spdlog::level::err:
        logger->error(msg);
        break;
      case spdlog::level::critical:
        logger->critical(msg);
        break;
      default:
        log_failure_error(std::format("Logger {} has invalid level. Dropped Log :\n{}", log_name, msg));
    }
  }

  void logger::send_log(spdlog::level::level_enum level, uint16_t sink_id, const std::string_view msg, std::source_location loc) {
    if (sink_id == 0) {
      log_failure_error(std::format("Sink ID is 0. Dropped Log :\n{}", msg));
      return;
    }

    auto sink_itr = sinks.find(sink_id);
    if (sink_itr == sinks.end()) {
      log_failure_error(std::format("Sink with ID {} not found. Dropped Log :\n{}", sink_id, msg));
      return;
    }

    auto& sink = sink_itr->second;
    if (sink.sink == nullptr) {
      log_failure_error(std::format("Sink with ID {} is null. Dropped Log :\n{}", sink_id, msg));
      return;
    }

    if (sink.sink->level() == spdlog::level::off) {
      return;
    }

    spdlog::source_loc loc_info = spdlog::source_loc{ loc.file_name(), (int)loc.line(), loc.function_name() };
    spdlog::details::log_msg log_msg(loc_info, sink.main_logger_name, level, msg);
    sink.sink->log(log_msg);
  }

  void logger::send_log(spdlog::level::level_enum level, const std::vector<uint16_t>& sink_id, const std::string_view msg, std::source_location loc) {
    if (sink_id.empty()) {
      log_failure_error(std::format("No sink IDs provided. Dropped Log :\n{}", msg));
      return;
    }

    for (const auto& id : sink_id) {
      send_log(level, id, msg, loc);
    }
  }

  void logger::log_failure_error(const std::string& message) {
    if (!error_log_file.is_open()) {
      error_log_file.open(kLogFailureFile.data(), std::ios::app | std::ios::out);
    }

    std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm now_tm = *std::localtime(&now_time);

    std::stringstream time_stream;
    time_stream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

    std::string time_str = time_stream.str();
    error_log_file << "[" << time_str << "] "
                   << "LOG FAILURE ERROR: " << message << std::endl;
  }

}  // namespace tensor