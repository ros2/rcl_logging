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
#include "rcl_logging_spdlog/logging_interface.h"
#include "gtest/gtest.h"

static void * bad_malloc(size_t, void *)
{
  return nullptr;
}

TEST(logging, init_invalid)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_allocator_t bad_allocator = rcutils_get_default_allocator();
  bad_allocator.allocate = bad_malloc;

  // Config files are not supported by spdlog
  ASSERT_NE(0, rcl_logging_external_initialize("anything", allocator));
  rcutils_reset_error();
  ASSERT_NE(0, rcl_logging_external_initialize(nullptr, bad_allocator));
}
