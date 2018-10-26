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

#ifndef RC_EXTERNAL_LOGGING_RC_EXTERNAL_LOGGING_H_
#define RC_EXTERNAL_LOGGING_RC_EXTERNAL_LOGGING_H_

#include <stdarg.h>
#include <rc_external_logging/visibility_control.h>


typedef int rc_external_logging_ret_t;
#define RC_EXTERNAL_LOGGING_RET_OK                          (0)
#define RC_EXTERNAL_LOGGING_RET_WARN                        (1)
#define RC_EXTERNAL_LOGGING_RET_ERROR                       (2)
#define RC_EXTERNAL_LOGGING_RET_INVALID_ARGUMENT            (11)
#define RC_EXTERNAL_LOGGING_RET_CONFIG_FILE_DOESNT_EXIST    (21)
#define RC_EXTERNAL_LOGGING_RET_CONFIG_FILE_INVALID         (22)

/* These are defined here to match the severity levels in rcutils. They provide a consistent way for external logger
    implementations to map between the incoming integer severity from ROS to the concept of DEBUG, INFO, WARN, ERROR,
    and FATAL*/
enum RC_EXTERNAL_LOGGING_LOG_SEVERITY
{
  RC_EXTERNAL_LOGGING_SEVERITY_UNSET = 0,  ///< The unset log level
  RC_EXTERNAL_LOGGING_SEVERITY_DEBUG = 10,  ///< The debug log level
  RC_EXTERNAL_LOGGING_SEVERITY_INFO = 20,  ///< The info log level
  RC_EXTERNAL_LOGGING_SEVERITY_WARN = 30,  ///< The warn log level
  RC_EXTERNAL_LOGGING_SEVERITY_ERROR = 40,  ///< The error log level
  RC_EXTERNAL_LOGGING_SEVERITY_FATAL = 50,  ///< The fatal log level
};

/**
 *  \brief Initializes the external logging library.
 *
 *  \param config_file The location of a config file that the external logging library should use to configure itself.
 *          If no config file is provided this will be set to an empty string. Must be a NULL terminated c string.
 *  \return RC_EXTERNAL_LOGGING_RET_OK if initialized successfully or an error code if not.
 */
RC_EXTERNAL_LOGGING_PUBLIC
rc_external_logging_ret_t rc_external_logging_initialize(const char * config_file);

/**
 *  \brief Free the resources allocated for the external logging system.
 *  This puts the system into a state equivalent to being uninitialized
 *
 *  \return RC_EXTERNAL_LOGGING_RET_OK if successfully shutdown or an error code if not.
 */
RC_EXTERNAL_LOGGING_PUBLIC
rc_external_logging_ret_t rc_external_logging_shutdown();

/**
 *  \brief Logs a message
 *
 *  \param severity The severity level of the message being logged
 *  \param name The name of the logger, must be a null terminated c string or NULL. If NULL or empty the root logger will
 *          be used.
 *  \param msg The message to be logged. Must be a null terminated c string.
 */
RC_EXTERNAL_LOGGING_PUBLIC
void rc_external_logging_log(int severity, const char * name, const char * msg);


/**
 *  \brief Set the severity level for a logger.
 *  Sets the severity level for the specified logger. If the name provided is an empty string or NULL it will change the
 *  level of the root logger.
 *
 *  \param name The name of the logger. Must be a NULL terminated c string or NULL.
 *  \param level - The severity level to be used for the specified logger
 *  \return RC_EXTERNAL_LOGGING_RET_OK if set successfully or an error code if not.
 */
RC_EXTERNAL_LOGGING_PUBLIC
rc_external_logging_ret_t rc_external_logging_set_logger_level(const char * name, int level);

#endif
