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
#include <rcutils/logging.h>
#include "rcl_logging_spdlog/logging_interface.h"
#include "gtest/gtest.h"

static void * bad_malloc(size_t, void *)
{
  return nullptr;
}

TEST(logging_interface, init_invalid)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_allocator_t bad_allocator = rcutils_get_default_allocator();
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  bad_allocator.allocate = bad_malloc;

  // Config files are not supported by spdlog
  EXPECT_EQ(2, rcl_logging_external_initialize("anything", allocator));
  rcutils_reset_error();
  EXPECT_EQ(2, rcl_logging_external_initialize(nullptr, bad_allocator));
  rcutils_reset_error();
  EXPECT_EQ(2, rcl_logging_external_initialize(nullptr, invalid_allocator));
  rcutils_reset_error();
}

TEST(logging_interface, full_cycle)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  ASSERT_EQ(0, rcl_logging_external_initialize(nullptr, allocator));
  EXPECT_EQ(0, rcl_logging_external_set_logger_level(nullptr, RCUTILS_LOG_SEVERITY_INFO));
  rcl_logging_external_log(RCUTILS_LOG_SEVERITY_INFO, nullptr, "Log Message");
  EXPECT_EQ(0, rcl_logging_external_shutdown());
}
