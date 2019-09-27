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

#include <cerrno>
#include <memory>
#include <inttypes.h>

#include <rcutils/allocator.h>
#include <rcutils/filesystem.h>
#include <rcutils/get_env.h>
#include <rcutils/process.h>
#include <rcutils/snprintf.h>
#include <rcutils/time.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "rcl_logging_spdlog/logging_interface.h"

#define RC_LOGGING_RET_OK                          (0)
#define RC_LOGGING_RET_ERROR                       (2)

#ifdef __cplusplus
extern "C" {
#endif

static std::shared_ptr<spdlog::logger> g_root_logger;

/* These are defined here to match the severity levels in rcl. They provide a consistent way for external logger
    implementations to map between the incoming integer severity from ROS to the concept of DEBUG, INFO, WARN, ERROR,
    and FATAL*/
enum RC_LOGGING_LOG_SEVERITY
{
  RC_LOGGING_SEVERITY_UNSET = 0,  ///< The unset log level
  RC_LOGGING_SEVERITY_DEBUG = 10,  ///< The debug log level
  RC_LOGGING_SEVERITY_INFO = 20,  ///< The info log level
  RC_LOGGING_SEVERITY_WARN = 30,  ///< The warn log level
  RC_LOGGING_SEVERITY_ERROR = 40,  ///< The error log level
  RC_LOGGING_SEVERITY_FATAL = 50,  ///< The fatal log level
};

static spdlog::level::level_enum map_external_log_level_to_library_level(int external_level)
{
  spdlog::level::level_enum level;

  // map to the next highest level of severity
  if (external_level <= RC_LOGGING_SEVERITY_DEBUG) {
    level = spdlog::level::level_enum::debug;
  } else if (external_level <= RC_LOGGING_SEVERITY_INFO) {
    level = spdlog::level::level_enum::info;
  } else if (external_level <= RC_LOGGING_SEVERITY_WARN) {
    level = spdlog::level::level_enum::warn;
  } else if (external_level <= RC_LOGGING_SEVERITY_ERROR) {
    level = spdlog::level::level_enum::err;
  } else if (external_level <= RC_LOGGING_SEVERITY_FATAL) {
    level = spdlog::level::level_enum::critical;
  }
  return level;
}

rcl_logging_ret_t rcl_logging_external_initialize(const char * config_file, rcutils_allocator_t allocator)
{
  bool config_file_provided = (nullptr != config_file) && (config_file[0] != '\0');
  if (config_file_provided) {
    // TODO(clalancette): implement support for an external configuration file.
    return RC_LOGGING_RET_ERROR;
  } else {
    // To be compatible with ROS 1, we construct a default filename of
    // the form ~/.ros/log/<exe>_<pid>_<milliseconds-since-epoch>.log

    // First get the home directory.
    const char *homedir = rcutils_get_home_dir();
    if (homedir == NULL) {
      // We couldn't get the home directory; it is not really going to be
      // possible to do logging properly, so get out of here without setting
      // up logging.
      return RC_LOGGING_RET_ERROR;
    }

    // SPDLOG doesn't automatically create the log directories, so make them
    // by hand here.
    char name_buffer[4096] = {0};
    int print_ret = rcutils_snprintf(name_buffer, sizeof(name_buffer), "%s/.ros", homedir);
    if (print_ret < 0) {
      return RC_LOGGING_RET_ERROR;
    }
    if (!rcutils_mkdir(name_buffer)) {
      return RC_LOGGING_RET_ERROR;
    }

    print_ret = rcutils_snprintf(name_buffer, sizeof(name_buffer), "%s/.ros/log", homedir);
    if (print_ret < 0) {
      return RC_LOGGING_RET_ERROR;
    }
    if (!rcutils_mkdir(name_buffer)) {
      return RC_LOGGING_RET_ERROR;
    }

    // Now get the milliseconds since the epoch in the local timezone.
    rcutils_time_point_value_t now;
    rcutils_ret_t ret = rcutils_system_time_now(&now);
    if (ret != RCUTILS_RET_OK) {
      // We couldn't get the system time, so get out of here without setting up
      // logging.
      return RC_LOGGING_RET_ERROR;
    }
    int64_t ms_since_epoch = RCUTILS_NS_TO_MS(now);

    // Get the program name.
    char *basec = rcutils_get_executable_name(allocator);
    if (basec == NULL) {
      // We couldn't get the program name, so get out of here without setting up
      // logging.
      return RC_LOGGING_RET_ERROR;
    }

    print_ret = rcutils_snprintf(name_buffer, sizeof(name_buffer), "%s/.ros/log/%s_%i_%" PRId64 ".log", homedir, basec, rcutils_get_pid(), ms_since_epoch);
    allocator.deallocate(basec, allocator.state);
    if (print_ret < 0) {
      return RC_LOGGING_RET_ERROR;
    }

    g_root_logger = spdlog::basic_logger_mt("root", name_buffer);
    g_root_logger->set_pattern("%v");
  }

  return RC_LOGGING_RET_OK;
}

rcl_logging_ret_t rcl_logging_external_shutdown()
{
  g_root_logger = nullptr;
  return RC_LOGGING_RET_OK;
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

  return RC_LOGGING_RET_OK;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
