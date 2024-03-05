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

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

#include "rcpputils/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/process.h"
#include "rcutils/strdup.h"

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

class LoggingTest : public AllocatorTest
{
public:
  LoggingTest()
  : AllocatorTest()
  {
  }

  std::filesystem::path find_single_log(const char * prefix)
  {
    char * rcl_log_dir = nullptr;
    rcl_logging_ret_t dir_ret = rcl_logging_get_logging_directory(allocator, &rcl_log_dir);
    if (dir_ret != RCL_LOGGING_RET_OK) {
      throw std::runtime_error("Failed to get logging directory");
    }
    RCPPUTILS_SCOPE_EXIT(
    {
      allocator.deallocate(rcl_log_dir, allocator.state);
    });
    std::filesystem::path log_dir(rcl_log_dir);
    std::string expected_prefix = get_expected_log_prefix(prefix);

    std::filesystem::path found;
    std::filesystem::file_time_type found_last_write;
    for (const std::filesystem::directory_entry & dir_entry :
      std::filesystem::directory_iterator{log_dir})
    {
      // If the start of the filename matches the expected_prefix, and this is the newest file
      // starting with that prefix, hold onto it to return later.
      if (dir_entry.path().filename().string().rfind(expected_prefix, 0) == 0) {
        if (found.string().empty() || dir_entry.last_write_time() > found_last_write) {
          found = dir_entry.path();
          found_last_write = dir_entry.last_write_time();
          // Even though we found the file, we have to keep looking in case there
          // is another file with the same prefix but a newer timestamp.
        }
      }
    }

    return found;
  }

private:
  std::string get_expected_log_prefix(const char * name)
  {
    char * exe_name;
    if (name == nullptr || name[0] == '\0') {
      exe_name = rcutils_get_executable_name(allocator);
    } else {
      exe_name = rcutils_strdup(name, allocator);
    }
    if (nullptr == exe_name) {
      throw std::runtime_error("Failed to determine executable name");
    }
    std::stringstream prefix;
    prefix << exe_name << "_" << rcutils_get_pid() << "_";
    allocator.deallocate(exe_name, allocator.state);
    return prefix.str();
  }
};

#endif  // FIXTURES_HPP_
