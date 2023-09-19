// Copyright 2019 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "rcpputils/filesystem_helper.hpp"
#include "rcpputils/env.hpp"
#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/logging.h"
#include "rcutils/process.h"
#include "rcutils/snprintf.h"
#include "rcutils/time.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "rcl_logging_interface/rcl_logging_interface.h"

static std::mutex g_logger_mutex;
static std::shared_ptr<spdlog::logger> g_root_logger = nullptr;

constexpr std::size_t DEFAULT_ROTATING_FILE_SIZE_BYTES = 100 * 1024 * 1024;
constexpr std::size_t DEFAULT_ROTATING_MAX_NUM_FILES = 5;

using std::size_t;

static spdlog::level::level_enum map_external_log_level_to_library_level(int external_level)
{
  spdlog::level::level_enum level = spdlog::level::level_enum::off;

  // map to the next highest level of severity
  if (external_level <= RCUTILS_LOG_SEVERITY_DEBUG) {
    level = spdlog::level::level_enum::debug;
  } else if (external_level <= RCUTILS_LOG_SEVERITY_INFO) {
    level = spdlog::level::level_enum::info;
  } else if (external_level <= RCUTILS_LOG_SEVERITY_WARN) {
    level = spdlog::level::level_enum::warn;
  } else if (external_level <= RCUTILS_LOG_SEVERITY_ERROR) {
    level = spdlog::level::level_enum::err;
  } else if (external_level <= RCUTILS_LOG_SEVERITY_FATAL) {
    level = spdlog::level::level_enum::critical;
  }
  return level;
}

namespace
{

RCL_LOGGING_INTERFACE_LOCAL
bool
get_should_use_old_flushing_behavior()
{
  const char * env_var_name = "RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR";

  try {
    std::string env_var_value = rcpputils::get_env_var(env_var_name);

    if (env_var_value.empty()) {
      // not set
      return false;
    }
    if ("0" == env_var_value) {
      // explicitly false
      return false;
    }
    if ("1" == env_var_value) {
      // explicitly true
      return true;
    }

    // unknown value
    throw std::runtime_error("unrecognized value: " + env_var_value);
  } catch (const std::runtime_error & error) {
    throw std::runtime_error(
            std::string("failed to get env var '") + env_var_name + "': " + error.what()
    );
  }
}

}  // namespace

static std::string get_string_env_var(const char * env_var_name)
{
  const char * env_var_value;
  const char * error_str;
  error_str = rcutils_get_env(env_var_name, &env_var_value);
  if (error_str != nullptr) {
    throw std::runtime_error(
            std::string(
              "Failed to get env var '") + env_var_name + "': " + error_str);
  }
  return env_var_value;  // Returns empty string for unset or empty env vars
}

static bool contained_in(
  const std::vector<std::string> & check_strings,
  const std::string & value_str)
{
  return std::find(check_strings.begin(), check_strings.end(), value_str) != check_strings.end();
}

static std::string join_quoted(
  const std::string & sep, const std::string & quote_chars,
  const std::vector<std::string> & list)
{
  if (list.empty()) {
    return "";
  }

  std::string joined = quote_chars + *(list.begin()) + quote_chars;
  std::for_each(
    list.begin() + 1, list.end(),
    [&](const auto & entry) {joined += sep + quote_chars + entry + quote_chars;});

  return joined;
}

static bool get_bool_env_var(const char * env_var_name)
{
  std::string value_str = get_string_env_var(env_var_name);
  std::vector<std::string> true_strings = {"1", "true", "TRUE"};
  std::vector<std::string> false_strings = {"0", "false", "FALSE", ""};

  if (contained_in(false_strings, value_str)) {
    return false;
  } else if (contained_in(true_strings, value_str)) {
    return true;
  } else {
    throw std::runtime_error(
            std::string(
              "Unrecognized value for '") + env_var_name + "': '" + value_str + "'. " +
            "Valid truthy values: " + join_quoted(", ", "'", true_strings) + ". " +
            "Valid falsy values: " + join_quoted(", ", "'", false_strings) + ". "
    );
  }
}

static size_t get_size_t_env_var(const char * env_var_name, const size_t default_val)
{
  std::string value_str = get_string_env_var(env_var_name);
  if (value_str.empty()) {
    return default_val;
  } else {
    int value = std::stoi(value_str);
    if (value < 0) {
      throw std::runtime_error(
              std::string("Env var must be positive '") + env_var_name + "': '" + value_str + "'. "
      );
    }
    return value;
  }
}

rcl_logging_ret_t rcl_logging_external_initialize(
  const char * config_file,
  rcutils_allocator_t allocator)
{
  std::lock_guard<std::mutex> lk(g_logger_mutex);
  // It is possible for this to get called more than once in a process (some of
  // the tests do this implicitly by calling rclcpp::init more than once).
  // If the logger is already setup, don't do anything.
  if (g_root_logger != nullptr) {
    return RCL_LOGGING_RET_OK;
  }

  bool config_file_provided = (nullptr != config_file) && (config_file[0] != '\0');
  if (config_file_provided) {
    // TODO(clalancette): implement support for an external configuration file.
    RCUTILS_SET_ERROR_MSG(
      "spdlog logging backend doesn't currently support external configuration");
    return RCL_LOGGING_RET_ERROR;
  } else {
    // check RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR to see if we
    // should change log file flushing behavior
    bool should_use_old_flushing_behavior = false;
    try {
      should_use_old_flushing_behavior = ::get_should_use_old_flushing_behavior();
    } catch (const std::runtime_error & error) {
      RCUTILS_SET_ERROR_MSG(error.what());
      return RCL_LOGGING_RET_ERROR;
    }

    // To be compatible with ROS 1, we construct a default filename of
    // the form ~/.ros/log/<exe>_<pid>_<milliseconds-since-epoch>.log

    char * logdir = nullptr;
    rcl_logging_ret_t dir_ret = rcl_logging_get_logging_directory(allocator, &logdir);
    if (RCL_LOGGING_RET_OK != dir_ret) {
      // We couldn't get the log directory, so get out of here without setting up
      // logging.
      RCUTILS_SET_ERROR_MSG("Failed to get logging directory");
      return dir_ret;
    }

    // SPDLOG doesn't automatically create the log directories, so create them
    rcpputils::fs::path logdir_path(logdir);
    if (!rcpputils::fs::create_directories(logdir_path)) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("Failed to create log directory: %s", logdir);
      allocator.deallocate(logdir, allocator.state);
      return RCL_LOGGING_RET_ERROR;
    }

    // Now get the milliseconds since the epoch in the local timezone.
    rcutils_time_point_value_t now;
    rcutils_ret_t ret = rcutils_system_time_now(&now);
    if (ret != RCUTILS_RET_OK) {
      allocator.deallocate(logdir, allocator.state);
      // We couldn't get the system time, so get out of here without setting up
      // logging.  We don't need to call RCUTILS_SET_ERROR_MSG either since
      // rcutils_system_time_now() already did it.
      return RCL_LOGGING_RET_ERROR;
    }
    int64_t ms_since_epoch = RCUTILS_NS_TO_MS(now);

    // Get the program name.
    char * basec = rcutils_get_executable_name(allocator);
    if (basec == nullptr) {
      allocator.deallocate(logdir, allocator.state);
      // We couldn't get the program name, so get out of here without setting up
      // logging.
      RCUTILS_SET_ERROR_MSG("Failed to get the executable name");
      return RCL_LOGGING_RET_ERROR;
    }

    char name_buffer[4096] = {0};
    int print_ret = rcutils_snprintf(
      name_buffer, sizeof(name_buffer),
      "%s/%s_%i_%" PRId64 ".log", logdir,
      basec, rcutils_get_pid(), ms_since_epoch);
    allocator.deallocate(logdir, allocator.state);
    allocator.deallocate(basec, allocator.state);
    if (print_ret < 0) {
      RCUTILS_SET_ERROR_MSG("Failed to create log file name string");
      return RCL_LOGGING_RET_ERROR;
    }

    std::unique_ptr<spdlog::sinks::sink> sink;

    try {
      if (get_bool_env_var("RCL_LOGGING_SPDLOG_ROTATE_FILES") == true) {
        size_t max_size =
          get_size_t_env_var(
          "RCL_LOGGING_SPDLOG_ROTATING_FILE_SIZE_BYTES",
          DEFAULT_ROTATING_FILE_SIZE_BYTES);
        size_t max_files =
          get_size_t_env_var(
          "RCL_LOGGING_SPDLOG_MAX_NUM_FILES",
          DEFAULT_ROTATING_MAX_NUM_FILES);
        sink =
          std::make_unique<spdlog::sinks::rotating_file_sink_mt>(name_buffer, max_size, max_files);
      } else {
        sink = std::make_unique<spdlog::sinks::basic_file_sink_mt>(name_buffer, false);
      }
    } catch (const std::runtime_error & error) {
      RCUTILS_SET_ERROR_MSG(error.what());
      return RCL_LOGGING_RET_ERROR;
    }

    g_root_logger = std::make_shared<spdlog::logger>("root", std::move(sink));
    if (!should_use_old_flushing_behavior) {
      // in this case we should do the new thing (until config files are supported)
      // which is to configure the logger to flush periodically and on
      // error level messages
      spdlog::flush_every(std::chrono::seconds(5));
      g_root_logger->flush_on(spdlog::level::err);
    } else {
      // the old behavior is to not configure the sink at all, so do nothing
    }

    spdlog::register_logger(g_root_logger);

    g_root_logger->set_pattern("%v");
  }

  return RCL_LOGGING_RET_OK;
}

rcl_logging_ret_t rcl_logging_external_shutdown()
{
  spdlog::drop("root");
  g_root_logger = nullptr;
  return RCL_LOGGING_RET_OK;
}

void rcl_logging_external_log(int severity, const char * name, const char * msg)
{
  (void)name;
  g_root_logger->log(map_external_log_level_to_library_level(severity), msg);
}

rcl_logging_ret_t rcl_logging_external_set_logger_level(const char * name, int level)
{
  (void)name;

  g_root_logger->set_level(map_external_log_level_to_library_level(level));

  return RCL_LOGGING_RET_OK;
}
