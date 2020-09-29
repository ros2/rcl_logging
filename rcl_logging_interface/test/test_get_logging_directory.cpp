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

#include <rcpputils/filesystem_helper.hpp>
#include <rcpputils/get_env.hpp>
#include <rcutils/allocator.h>
#include <rcutils/env.h>

#include <string>

#include "gtest/gtest.h"
#include "rcl_logging_interface/rcl_logging_interface.h"

// This is a helper class that resets an environment
// variable when leaving scope
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
    if (!rcutils_set_env(name_.c_str(), value_.c_str())) {
      std::cerr << "Failed to restore value of environment variable: " << name_ << std::endl;
    }
  }

private:
  const std::string name_;
  const std::string value_;
};

TEST(test_logging_directory, directory)
{
  RestoreEnvVar home_var("HOME");
  RestoreEnvVar userprofile_var("USERPROFILE");
  ASSERT_EQ(true, rcutils_set_env("HOME", nullptr));
  ASSERT_EQ(true, rcutils_set_env("USERPROFILE", nullptr));
  ASSERT_EQ(true, rcutils_set_env("ROS2_LOG_DIR", nullptr));
  ASSERT_EQ(true, rcutils_set_env("ROS2_HOME", nullptr));

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // Fails without any relevant env vars at all (HOME included)
  const char * directory;
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_get_logging_directory(&allocator, &directory));

  // Default case without ROS2_LOG_DIR or ROS2_HOME being set (but with HOME)
  rcpputils::fs::path fake_home("/fake_home_dir");
  ASSERT_EQ(true, rcutils_set_env("HOME", fake_home.string().c_str()));
  rcpputils::fs::path default_dir = fake_home / ".ros" / "log";
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());

  // Use $ROS2_LOG_DIR if it is set
  ASSERT_EQ(true, rcutils_set_env("ROS2_LOG_DIR", "/my/ros2_log_dir"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  EXPECT_STREQ(directory, "/my/ros2_log_dir");
  // Empty is considered unset
  ASSERT_EQ(true, rcutils_set_env("ROS2_LOG_DIR", ""));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());
  // Make sure '~' is expanded to the home directory
  ASSERT_EQ(true, rcutils_set_env("ROS2_LOG_DIR", "~/logdir"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  rcpputils::fs::path fake_log_dir = fake_home / "logdir";
  EXPECT_STREQ(directory, fake_log_dir.string().c_str());

  ASSERT_EQ(true, rcutils_set_env("ROS2_LOG_DIR", nullptr));

  // Without ROS2_LOG_DIR, use $ROS2_HOME/log
  rcpputils::fs::path fake_ros_home = fake_home / ".fakeroshome";
  ASSERT_EQ(true, rcutils_set_env("ROS2_HOME", fake_ros_home.string().c_str()));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  rcpputils::fs::path fake_ros_home_log_dir = fake_ros_home / "log";
  EXPECT_STREQ(directory, fake_ros_home_log_dir.string().c_str());
  // Empty is considered unset
  ASSERT_EQ(true, rcutils_set_env("ROS2_HOME", ""));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());
  // Make sure '~' is expanded to the home directory
  ASSERT_EQ(true, rcutils_set_env("ROS2_HOME", "~/.fakeroshome"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(&allocator, &directory));
  EXPECT_STREQ(directory, fake_ros_home_log_dir.string().c_str());

  ASSERT_EQ(true, rcutils_set_env("ROS2_HOME", nullptr));
}
