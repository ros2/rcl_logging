// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <rcutils/allocator.h>
#include <rcutils/error_handling.h>
#include <rcutils/filesystem.h>
#include <rcutils/find.h>
#include <rcutils/get_env.h>
#include <rcutils/strdup.h>

#include "rcl_logging_interface/rcl_logging_interface.h"

rcl_logging_ret_t
rcl_logging_get_logging_directory(rcutils_allocator_t allocator, char ** directory)
{
  if (NULL == directory) {
    RCUTILS_SET_ERROR_MSG("directory argument must not be null");
    return RCL_LOGGING_RET_INVALID_ARGUMENT;
  }
  if (NULL != *directory) {
    RCUTILS_SET_ERROR_MSG("directory argument must point to null");
    return RCL_LOGGING_RET_INVALID_ARGUMENT;
  }

  const char * log_dir_env;
  const char * err = rcutils_get_env("ROS_LOG_DIR", &log_dir_env);
  if (NULL != err) {
    RCUTILS_SET_ERROR_MSG("rcutils_get_env failed");
    return RCL_LOGGING_RET_ERROR;
  }
  if ('\0' != *log_dir_env) {
    *directory = rcutils_strdup(log_dir_env, allocator);
    if (NULL == *directory) {
      RCUTILS_SET_ERROR_MSG("rcutils_strdup failed");
      return RCL_LOGGING_RET_ERROR;
    }
  } else {
    const char * ros_home_dir_env;
    err = rcutils_get_env("ROS_HOME", &ros_home_dir_env);
    if (NULL != err) {
      RCUTILS_SET_ERROR_MSG("rcutils_get_env failed");
      return RCL_LOGGING_RET_ERROR;
    }
    if ('\0' == *ros_home_dir_env) {
      ros_home_dir_env = rcutils_join_path("~", ".ros", allocator);
      if (NULL == ros_home_dir_env) {
        RCUTILS_SET_ERROR_MSG("rcutils_join_path failed");
        return RCL_LOGGING_RET_ERROR;
      }
    }
    *directory = rcutils_join_path(ros_home_dir_env, "log", allocator);
    if (NULL == *directory) {
      RCUTILS_SET_ERROR_MSG("rcutils_join_path failed");
      return RCL_LOGGING_RET_ERROR;
    }
  }

  // Expand home directory
  if (0 == rcutils_find(*directory, '~')) {
    const char * homedir = rcutils_get_home_dir();
    if (NULL == homedir) {
      RCUTILS_SET_ERROR_MSG("failed to get the home directory");
      return RCL_LOGGING_RET_ERROR;
    }
    char * directory_not_expanded = *directory;
    *directory = rcutils_join_path(homedir, directory_not_expanded + 2, allocator);
    allocator.deallocate(directory_not_expanded, allocator.state);
    if (NULL == *directory) {
      RCUTILS_SET_ERROR_MSG("rcutils_join_path failed");
      return RCL_LOGGING_RET_ERROR;
    }
  }
  return RCL_LOGGING_RET_OK;
}
