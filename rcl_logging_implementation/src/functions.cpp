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

#include "functions.hpp"

#include <atomic>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include "rcl_logging_interface/rcl_logging_interface.h"

#include "rcpputils/env.hpp"
#include "rcpputils/scope_exit.hpp"
#include "rcpputils/shared_library.hpp"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/logging_macros.h"

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

// Default logging implementation
#ifndef DEFAULT_RCL_LOGGING_IMPLEMENTATION
#define DEFAULT_RCL_LOGGING_IMPLEMENTATION rcl_logging_spdlog
#endif

// Function pointer types
typedef rcl_logging_ret_t (* rcl_logging_initialize_func_t)(
  const char *, const char *, rcutils_allocator_t);
typedef rcl_logging_ret_t (* rcl_logging_shutdown_func_t)(void);
typedef void (* rcl_logging_log_func_t)(int, const char *, const char *);
typedef rcl_logging_ret_t (* rcl_logging_set_logger_level_func_t)(const char *, int);

// Global state
static std::shared_ptr<rcpputils::SharedLibrary> g_logging_lib = nullptr;
static rcl_logging_initialize_func_t g_initialize_func = nullptr;
static rcl_logging_shutdown_func_t g_shutdown_func = nullptr;
static rcl_logging_log_func_t g_log_func = nullptr;
static rcl_logging_set_logger_level_func_t g_set_logger_level_func = nullptr;
static std::once_flag g_atexit_once_flag;

static std::shared_ptr<rcpputils::SharedLibrary>
attempt_to_load_one_logging_library(const std::string & library)
{
  std::string library_name;
  std::shared_ptr<rcpputils::SharedLibrary> ret = nullptr;

  try {
    library_name = rcpputils::get_platform_library_name(library);
  } catch (const std::exception & e) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to compute shared library name due to %s", e.what());
    return ret;
  }

  try {
    ret = std::make_shared<rcpputils::SharedLibrary>(library_name);
    RCUTILS_LOG_DEBUG_NAMED(
      "rcl_logging_implementation",
      "Successfully loaded library: %s", library_name.c_str());
  } catch (const std::exception & e) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to load shared library '%s' due to %s",
      library_name.c_str(), e.what());
    ret = nullptr;
  }

  return ret;
}

static void *
lookup_symbol(
  std::shared_ptr<rcpputils::SharedLibrary> lib,
  const std::string & symbol_name)
{
  if (!lib) {
    RCUTILS_SET_ERROR_MSG("no shared library to lookup");
    return nullptr;
  }

  if (!lib->has_symbol(symbol_name)) {
    try {
      std::string library_path = lib->get_library_path();
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "failed to resolve symbol '%s' in shared library '%s'",
        symbol_name.c_str(), library_path.c_str());
    } catch (const std::exception & e) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "failed to resolve symbol '%s' in shared library due to %s",
        symbol_name.c_str(), e.what());
    }
    return nullptr;
  }
  return lib->get_symbol(symbol_name);
}

bool
load_logging_library()
{
  // Already loaded
  if (g_logging_lib) {
    return true;
  }

  // The logic to pick the logging library to load goes as follows:
  //
  // 1. If the user specified the library to use via the RCL_LOGGING_IMPLEMENTATION
  //    environment variable, try to load only that library.
  // 2. Otherwise, try to load the default logging implementation.

  std::string env_var;
  try {
    env_var = rcpputils::get_env_var("RCL_LOGGING_IMPLEMENTATION");
  } catch (const std::exception & e) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to fetch RCL_LOGGING_IMPLEMENTATION "
      "from environment due to %s", e.what());
    return false;
  }

  // User specified a logging implementation, attempt to load that one and only that one
  if (!env_var.empty()) {
    g_logging_lib = attempt_to_load_one_logging_library(env_var);
  } else {
    RCUTILS_LOG_DEBUG_NAMED(
      "rcl_logging_implementation",
      "RCL_LOGGING_IMPLEMENTATION not set, using default: %s",
      STRINGIFY(DEFAULT_RCL_LOGGING_IMPLEMENTATION));

    // User didn't specify, so load the default logging implementation
    g_logging_lib =
      attempt_to_load_one_logging_library(STRINGIFY(DEFAULT_RCL_LOGGING_IMPLEMENTATION));
  }

  if (!g_logging_lib) {
    RCUTILS_SET_ERROR_MSG("failed to load any logging implementations");
    return false;
  }

  // Set up cleanup handler in case of failure
  bool success = false;
  RCPPUTILS_SCOPE_EXIT(
  {
    if (!success) {
      g_logging_lib.reset();
      g_initialize_func = nullptr;
      g_shutdown_func = nullptr;
      g_log_func = nullptr;
      g_set_logger_level_func = nullptr;
    }
  });

  // Register all function pointers
  g_initialize_func = reinterpret_cast<rcl_logging_initialize_func_t>(
    lookup_symbol(g_logging_lib, "rcl_logging_external_initialize"));
  if (!g_initialize_func) {
    return false;
  }

  g_shutdown_func = reinterpret_cast<rcl_logging_shutdown_func_t>(
    lookup_symbol(g_logging_lib, "rcl_logging_external_shutdown"));
  if (!g_shutdown_func) {
    return false;
  }

  g_log_func = reinterpret_cast<rcl_logging_log_func_t>(
    lookup_symbol(g_logging_lib, "rcl_logging_external_log"));
  if (!g_log_func) {
    return false;
  }

  g_set_logger_level_func = reinterpret_cast<rcl_logging_set_logger_level_func_t>(
    lookup_symbol(g_logging_lib, "rcl_logging_external_set_logger_level"));
  if (!g_set_logger_level_func) {
    return false;
  }

  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "Successfully registered all function pointers from logging library");

  // Register atexit handler to unload library at process exit
  // This avoids race conditions during shutdown while ensuring cleanup
  // std::call_once guarantees the handler is registered exactly once
  std::call_once(
    g_atexit_once_flag, []() {std::atexit(unload_logging_library);}
  );

  success = true;
  return true;
}

#ifdef __cplusplus
extern "C"
{
#endif

rcl_logging_ret_t
rcl_logging_external_initialize(
  const char * file_name_prefix,
  const char * config_file,
  rcutils_allocator_t allocator)
{
  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "rcl_logging_external_initialize called (prefix: %s, config: %s)",
    NULL != file_name_prefix ? file_name_prefix : "NULL",
    NULL != config_file ? config_file : "NULL");

  // Load library and register all function pointers
  if (!load_logging_library()) {
    // error message already set by load_logging_library()
    return RCL_LOGGING_RET_ERROR;
  }

  return g_initialize_func(file_name_prefix, config_file, allocator);
}

rcl_logging_ret_t
rcl_logging_external_shutdown(void)
{
  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "rcl_logging_external_shutdown called");

  if (!g_shutdown_func) {
    // If shutdown is called before init, it's OK to just return success
    return RCL_LOGGING_RET_OK;
  }

  rcl_logging_ret_t ret = g_shutdown_func();

  // Note: We intentionally do NOT unload the library here to avoid race conditions.
  // The library will be unloaded at process exit via the registered atexit handler.
  // This ensures that function pointers remain valid if any thread is still logging.
  // With this approach, it does not have to generate the mutex overhead.

  return ret;
}

void
rcl_logging_external_log(int severity, const char * name, const char * msg)
{
  if (!g_log_func) {
    // If log is called before init, just return silently
    return;
  }

  g_log_func(severity, name, msg);
}

rcl_logging_ret_t
rcl_logging_external_set_logger_level(const char * name, int level)
{
  if (!g_set_logger_level_func) {
    RCUTILS_SET_ERROR_MSG("logging library not initialized");
    return RCL_LOGGING_RET_ERROR;
  }

  return g_set_logger_level_func(name, level);
}

#ifdef __cplusplus
}
#endif

void
unload_logging_library()
{
  RCUTILS_LOG_DEBUG_NAMED("rcl_logging_implementation", "Unloading logging library");
  g_initialize_func = nullptr;
  g_shutdown_func = nullptr;
  g_log_func = nullptr;
  g_set_logger_level_func = nullptr;
  g_logging_lib.reset();
}

void
force_unload_logging_library()
{
  // Call the regular unload function
  unload_logging_library();
  // Reset the once_flag to allow re-registration of atexit handler
  // This is safe in test scenarios but should NOT be used in production
  // Note: std::once_flag doesn't have a standard reset method, so we use placement new
  // to reconstruct it in-place, which is a well-known pattern for testing
  g_atexit_once_flag.~once_flag();
  new (&g_atexit_once_flag) std::once_flag();
}
