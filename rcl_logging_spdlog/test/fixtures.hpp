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

#ifndef FIXTURES_HPP_
#define FIXTURES_HPP_

#include <rcutils/allocator.h>
#include "gtest/gtest.h"

class AllocatorTest : public ::testing::Test
{
public:
  AllocatorTest()
  : allocator(rcutils_get_default_allocator()),
    bad_allocator(get_bad_allocator()),
    invalid_allocator(rcutils_get_zero_initialized_allocator())
  {
  }

  rcutils_allocator_t allocator;
  rcutils_allocator_t bad_allocator;
  rcutils_allocator_t invalid_allocator;

private:
  static rcutils_allocator_t get_bad_allocator()
  {
    rcutils_allocator_t bad_allocator = rcutils_get_default_allocator();
    bad_allocator.allocate = AllocatorTest::bad_malloc;
    bad_allocator.reallocate = AllocatorTest::bad_realloc;
    return bad_allocator;
  }

  static void * bad_malloc(size_t, void *)
  {
    return nullptr;
  }

  static void * bad_realloc(void *, size_t, void *)
  {
    return nullptr;
  }
};

#endif  // FIXTURES_HPP_
