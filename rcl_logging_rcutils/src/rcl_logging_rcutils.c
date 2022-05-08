// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#include <rcl_logging_interface/rcl_logging_interface.h>
#include <rcutils/allocator.h>
#include <rcutils/logging.h>

rcl_logging_ret_t rcl_logging_external_initialize(
  const char * config_file,
  rcutils_allocator_t allocator)
{
  (void) config_file;
  rcutils_ret_t ret = rcutils_logging_initialize_with_allocator(allocator);

  if (RCUTILS_RET_OK != ret) {
    return RCL_LOGGING_RET_ERROR;
  }
  return RCL_LOGGING_RET_OK;
}

rcl_logging_ret_t rcl_logging_external_shutdown()
{
  rcutils_ret_t ret = rcutils_logging_shutdown();
  if (RCUTILS_RET_OK != ret) {
    return RCL_LOGGING_RET_ERROR;
  }
  return RCL_LOGGING_RET_OK;
}

void rcl_logging_external_log(int severity, const char * name, const char * msg)
{
  rcutils_log(NULL, severity, name, "%s", msg);
}

rcl_logging_ret_t rcl_logging_external_set_logger_level(const char * name, int level)
{
  rcutils_ret_t ret = rcutils_logging_set_logger_level(name, level);
  if (RCUTILS_RET_OK != ret) {
    return RCL_LOGGING_RET_ERROR;
  }
  return RCL_LOGGING_RET_OK;
}
