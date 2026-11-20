// Copyright 2026 Blueye Robotics AS
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

#include <cstdlib>
#include <filesystem>
#include <string>
#include <system_error>

#include "gtest/gtest.h"

#include "rcl_logging_interface/rcl_logging_interface.h"

#include "rcpputils/env.hpp"

#include "rcutils/allocator.h"
#include "rcutils/logging.h"
#include "rcutils/process.h"

#include "spdlog/spdlog.h"

class TestExitWithSpdlog : public ::testing::Test
{
public:
  void SetUp() override
  {
    log_dir = std::filesystem::temp_directory_path() /
      ("rcl_logging_test_exit_" + std::to_string(rcutils_get_pid()));
    std::filesystem::create_directories(log_dir);
    ASSERT_TRUE(rcpputils::set_env_var("ROS_LOG_DIR", log_dir.string().c_str()));
    ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));
  }

  void TearDown() override
  {
    std::error_code ec;
    std::filesystem::remove_all(log_dir, ec);
  }

  std::filesystem::path log_dir;
};

// Logs through rcl_logging_spdlog in a process that uses spdlog itself, then
// exits, after a shutdown or without one. The process's spdlog registry is
// created first, as in a program that logs with spdlog before ROS starts, so
// it is destroyed last, after any atexit() handler. If the logging library
// were unloaded before then, the flush thread and the logger that it left
// with spdlog would run its code after it was gone.
static void log_and_exit(bool shut_down)
{
  spdlog::default_logger()->flush();
  if (RCL_LOGGING_RET_OK != rcl_logging_external_initialize(
      nullptr, nullptr, rcutils_get_default_allocator()))
  {
    std::exit(2);
  }
  rcl_logging_external_log(RCUTILS_LOG_SEVERITY_INFO, nullptr, "Test message");
  if (shut_down && RCL_LOGGING_RET_OK != rcl_logging_external_shutdown()) {
    std::exit(3);
  }
  std::exit(0);
}

TEST_F(TestExitWithSpdlog, exit_after_shutdown)
{
  EXPECT_EXIT(log_and_exit(true), ::testing::ExitedWithCode(0), "");
}

TEST_F(TestExitWithSpdlog, exit_without_shutdown)
{
  EXPECT_EXIT(log_and_exit(false), ::testing::ExitedWithCode(0), "");
}
