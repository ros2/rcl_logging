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

#include <fstream>
#include <string>

#include "fixtures.hpp"
#include "gtest/gtest.h"
#include "rcl_logging_spdlog/logging_interface.h"

const int logger_levels[] =
{
  RCUTILS_LOG_SEVERITY_UNSET,
  RCUTILS_LOG_SEVERITY_DEBUG,
  RCUTILS_LOG_SEVERITY_INFO,
  RCUTILS_LOG_SEVERITY_WARN,
  RCUTILS_LOG_SEVERITY_ERROR,
  RCUTILS_LOG_SEVERITY_FATAL,
};

TEST_F(LoggingTest, init_invalid)
{
  // Config files are not supported by spdlog
  EXPECT_EQ(2, rcl_logging_external_initialize("anything", allocator));
  rcutils_reset_error();
  EXPECT_EQ(2, rcl_logging_external_initialize(nullptr, bad_allocator));
  rcutils_reset_error();
  EXPECT_EQ(2, rcl_logging_external_initialize(nullptr, invalid_allocator));
  rcutils_reset_error();
}

TEST_F(LoggingTest, full_cycle)
{
  ASSERT_EQ(0, rcl_logging_external_initialize(nullptr, allocator));

  // Make sure we can call initialize more than once
  ASSERT_EQ(0, rcl_logging_external_initialize(nullptr, allocator));

  std::stringstream expected_log;
  for (int level : logger_levels) {
    EXPECT_EQ(0, rcl_logging_external_set_logger_level(nullptr, level));

    for (int severity : logger_levels) {
      std::stringstream ss;
      ss << "Message of severity " << severity << " at level " << level;
      rcl_logging_external_log(severity, nullptr, ss.str().c_str());

      if (severity >= level) {
        expected_log << ss.str() << std::endl;
      } else if (severity == 0 && level == 10) {
        // This is a special case - not sure what the right behavior is
        expected_log << ss.str() << std::endl;
      }
    }
  }

  EXPECT_EQ(0, rcl_logging_external_shutdown());

  std::string log_file_path = find_single_log().string();
  std::ifstream log_file(log_file_path);
  std::stringstream actual_log;
  actual_log << log_file.rdbuf();
  EXPECT_EQ(
    expected_log.str(),
    actual_log.str()) << "Unexpected log contents in " << log_file_path;
}
