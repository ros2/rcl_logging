// Copyright 2025 Sony Group Corporation.
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

#include <chrono>
#include <filesystem>
#include <string>

#include "gtest/gtest.h"

#include "rcl_logging_interface/rcl_logging_interface.h"

#include "../src/functions.hpp"

#include "rcpputils/env.hpp"
#include "rcpputils/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/logging.h"

// Helper class to restore environment variable
class RestoreEnvVar
{
public:
  explicit RestoreEnvVar(const std::string & name)
  : name_(name),
    value_(rcpputils::get_env_var(name.c_str()))
  {
  }

  ~RestoreEnvVar()
  {
    if (!rcpputils::set_env_var(name_.c_str(), value_.c_str())) {
      std::cerr << "Failed to restore value of environment variable: " << name_ << std::endl;
    }
  }

private:
  const std::string name_;
  const std::string value_;
};

class TestLoggingImplementation : public ::testing::Test
{
public:
  void SetUp() override
  {
    allocator = rcutils_get_default_allocator();

    // Create a unique temporary directory for this test
    temp_log_dir = create_temp_directory();

    // Set ROS_LOG_DIR to the temporary directory
    ros_log_dir_restore = std::make_unique<RestoreEnvVar>("ROS_LOG_DIR");
    ASSERT_TRUE(rcpputils::set_env_var("ROS_LOG_DIR", temp_log_dir.c_str()));
  }

  void TearDown() override
  {
    // Force unload the logging library to allow tests to load different implementations
    force_unload_logging_library();

    // Clean up any leftover error state
    if (rcutils_error_is_set()) {
      rcutils_reset_error();
    }

    // Restore ROS_LOG_DIR environment variable
    ros_log_dir_restore.reset();

    // Remove the temporary directory and its contents
    if (!temp_log_dir.empty() && std::filesystem::exists(temp_log_dir)) {
      std::error_code ec;
      std::filesystem::remove_all(temp_log_dir, ec);
      if (ec) {
        std::cerr << "Failed to remove temporary directory: " << temp_log_dir
                  << " - " << ec.message() << std::endl;
      }
    }
  }

  std::string create_temp_directory()
  {
    // Use timestamp-based directory name for cross-platform compatibility
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
      now.time_since_epoch()).count();

    std::string temp_dir =
      std::filesystem::temp_directory_path().string() + "/rcl_logging_test_" +
      std::to_string(timestamp);
    std::filesystem::create_directories(temp_dir);
    return temp_dir;
  }

  rcutils_allocator_t allocator;
  std::string temp_log_dir;
  std::unique_ptr<RestoreEnvVar> ros_log_dir_restore;
};

TEST_F(TestLoggingImplementation, default_implementation)
{
  // Without setting RCL_LOGGING_IMPLEMENTATION, should default to rcl_logging_spdlog
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", ""));

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, explicit_spdlog)
{
  // Explicitly request rcl_logging_spdlog
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, explicit_noop)
{
  // Explicitly request rcl_logging_noop
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_noop"));

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, invalid_implementation)
{
  // Request a non-existent implementation
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "nonexistent_impl"));

  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();
}

TEST_F(TestLoggingImplementation, multiple_initialize_same_impl)
{
  // Multiple initializations with the same implementation should work
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, logging_functions)
{
  // Test that we can actually call logging functions
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));

  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));

  // These should not crash
  rcl_logging_external_log(RCUTILS_LOG_SEVERITY_INFO, "test_logger", "Test message");
  EXPECT_EQ(
    RCL_LOGGING_RET_OK,
    rcl_logging_external_set_logger_level("test_logger", RCUTILS_LOG_SEVERITY_DEBUG));

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, file_name_prefix)
{
  // Test with custom file name prefix
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));

  EXPECT_EQ(
    RCL_LOGGING_RET_OK,
    rcl_logging_external_initialize("my_custom_prefix", nullptr, allocator));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, severity_levels)
{
  // Test all severity levels
  RestoreEnvVar env_var("RCL_LOGGING_IMPLEMENTATION");
  ASSERT_TRUE(rcpputils::set_env_var("RCL_LOGGING_IMPLEMENTATION", "rcl_logging_spdlog"));

  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, nullptr, allocator));

  const int severity_levels[] = {
    RCUTILS_LOG_SEVERITY_UNSET,
    RCUTILS_LOG_SEVERITY_DEBUG,
    RCUTILS_LOG_SEVERITY_INFO,
    RCUTILS_LOG_SEVERITY_WARN,
    RCUTILS_LOG_SEVERITY_ERROR,
    RCUTILS_LOG_SEVERITY_FATAL,
  };

  for (int level : severity_levels) {
    EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_set_logger_level(nullptr, level));
    rcl_logging_external_log(level, nullptr, "Test message");
  }

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
}

TEST_F(TestLoggingImplementation, shutdown_without_initialize)
{
  // Calling shutdown without initialize should handle gracefully
  // (might return error or OK depending on implementation)
  rcl_logging_ret_t ret = rcl_logging_external_shutdown();
  EXPECT_EQ(RCL_LOGGING_RET_OK, ret);
  if (rcutils_error_is_set()) {
    rcutils_reset_error();
  }
}
