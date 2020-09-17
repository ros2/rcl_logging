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
#include <rcutils/logging.h>

#include <rcl_logging_interface/rcl_logging_interface.h>

#include <rmw/types.h>

#include <string>

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

namespace
{
constexpr const uint64_t kSize = 4096;
}


const int logger_levels[] =
{
  RCUTILS_LOG_SEVERITY_UNSET,
  RCUTILS_LOG_SEVERITY_DEBUG,
  RCUTILS_LOG_SEVERITY_INFO,
  RCUTILS_LOG_SEVERITY_WARN,
  RCUTILS_LOG_SEVERITY_ERROR,
  RCUTILS_LOG_SEVERITY_FATAL,
};

class RCL_LOGGING_BENCHMARK : public PerformanceTest
{
public:
  void SetUp(benchmark::State & st)
  {
    performance_test_fixture::PerformanceTest::SetUp(st);
    allocator = rcutils_get_default_allocator();

    rmw_ret_t ret = rcl_logging_external_initialize(nullptr, allocator);
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }

    data = std::string(kSize, '0');
    i = 0;
  }
  void TearDown(benchmark::State & st)
  {
    performance_test_fixture::PerformanceTest::TearDown(st);
    rmw_ret_t ret = rcl_logging_external_shutdown();
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  void setLogLevel(int logger_level, benchmark::State & st)
  {
    rmw_ret_t ret = rcl_logging_external_set_logger_level(nullptr, logger_level);
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  rcutils_allocator_t allocator;
  std::string data;
  int i;
};

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_unset)(benchmark::State & st)
{
  setLogLevel(logger_levels[0], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_unset);

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_debug)(benchmark::State & st)
{
  setLogLevel(logger_levels[1], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_debug);

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_info)(benchmark::State & st)
{
  setLogLevel(logger_levels[2], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_info);

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_warn)(benchmark::State & st)
{
  setLogLevel(logger_levels[3], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_warn);

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_error)(benchmark::State & st)
{
  setLogLevel(logger_levels[4], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_error);

BENCHMARK_DEFINE_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_fatal)(benchmark::State & st)
{
  setLogLevel(logger_levels[5], st);
  for (auto _ : st) {
    rcl_logging_external_log(i++ % 5, nullptr, data.c_str());
  }
}
BENCHMARK_REGISTER_F(RCL_LOGGING_BENCHMARK, benchmark_log_severity_fatal);
