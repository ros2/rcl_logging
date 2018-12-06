// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/file.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/patternlayout.h>


#if defined _WIN32 || defined __CYGWIN__
    #include <Windows.h>
    #define GET_PID() ((int)GetCurrentProcessId())
/* TODO(nburek): Change this to default to the %appdata% folder on Windows */
    #define DEFAULT_LOG_FILE    "ros_logs/%i.log"
#else
    #include <unistd.h>
    #define GET_PID() ((int)getpid())
    #define DEFAULT_LOG_FILE    "/var/log/ros/%i.log"
#endif

extern "C" {

typedef int rcl_logging_ret_t;
#define RC_LOGGING_RET_OK                          (0)
#define RC_LOGGING_RET_WARN                        (1)
#define RC_LOGGING_RET_ERROR                       (2)
#define RC_LOGGING_RET_INVALID_ARGUMENT            (11)
#define RC_LOGGING_RET_CONFIG_FILE_DOESNT_EXIST    (21)
#define RC_LOGGING_RET_CONFIG_FILE_INVALID         (22)

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

/**
 *  Maps the logger name to the log4cxx logger. If the name is null or empty it will map to the
 *  root logger.
 */
static const log4cxx::LoggerPtr get_logger(const char * name)
{
  if (nullptr == name || '\0' == name[0]) {
    return log4cxx::Logger::getRootLogger();
  }
  return log4cxx::Logger::getLogger(name);
}

static const log4cxx::LevelPtr map_external_log_level_to_library_level(int external_level)
{
  log4cxx::LevelPtr level;
  // map to the next highest level of severity
  if (external_level <= RC_LOGGING_SEVERITY_DEBUG) {
    level = log4cxx::Level::getDebug();
  } else if (external_level <= RC_LOGGING_SEVERITY_INFO) {
    level = log4cxx::Level::getInfo();
  } else if (external_level <= RC_LOGGING_SEVERITY_WARN) {
    level = log4cxx::Level::getWarn();
  } else if (external_level <= RC_LOGGING_SEVERITY_ERROR) {
    level = log4cxx::Level::getError();
  } else if (external_level <= RC_LOGGING_SEVERITY_FATAL) {
    level = log4cxx::Level::getFatal();
  }
  return level;
}

rcl_logging_ret_t rcl_logging_external_initialize(const char * config_file)
{
  log4cxx::File file(config_file);
  log4cxx::helpers::Pool pool;
  bool config_file_provided = (nullptr != config_file) && (config_file[0] != '\0');
  bool use_default_config = !config_file_provided;
  rcl_logging_ret_t status = RC_LOGGING_RET_OK;

  if (config_file_provided && !file.exists(pool)) {
    // The provided config file doesn't exist, fall back to using default configuration
    use_default_config = true;
    status = RC_LOGGING_RET_CONFIG_FILE_DOESNT_EXIST;
  } else if (config_file_provided) {
    // Attempt to configure the system using the provided config file, but if we fail fall back to using the default
    // configuration
    try {
      log4cxx::PropertyConfigurator::configure(file);
    } catch (std::exception & ex) {
      use_default_config = true;
      status = RC_LOGGING_RET_CONFIG_FILE_INVALID;
    }
  }

  if (use_default_config) {
    // Set the default File Appender on the root logger
    log4cxx::LoggerPtr root_logger(get_logger(nullptr));
    log4cxx::LayoutPtr layout(new log4cxx::PatternLayout("%m%n"));
    char log_name_buffer[64];
    snprintf(log_name_buffer, sizeof(log_name_buffer), DEFAULT_LOG_FILE, GET_PID());
    log4cxx::FileAppenderPtr file_appender(new log4cxx::FileAppender(layout, log_name_buffer,
      true));
    root_logger->addAppender(file_appender);
  }

  return status;
}

rcl_logging_ret_t rcl_logging_external_shutdown()
{
  log4cxx::BasicConfigurator::resetConfiguration();
  return RC_LOGGING_RET_OK;
}

void rcl_logging_external_log(int severity, const char * name, const char * msg)
{
  log4cxx::LoggerPtr logger(get_logger(name));
  log4cxx::LevelPtr level(map_external_log_level_to_library_level(severity));
  logger->log(level, msg);
}

rcl_logging_ret_t rcl_logging_external_set_logger_level(const char * name, int level)
{
  log4cxx::LoggerPtr logger(get_logger(name));
  logger->setLevel(map_external_log_level_to_library_level(level));
  return RC_LOGGING_RET_OK;
}

} /* extern "C" */
