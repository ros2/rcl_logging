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

#include <memory>
#include <stdexcept>
#include <string>

#include "rcl_logging_interface/rcl_logging_interface.h"

#include "rcpputils/env.hpp"
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

static std::shared_ptr<rcpputils::SharedLibrary> g_logging_lib = nullptr;

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

std::shared_ptr<rcpputils::SharedLibrary>
load_logging_library()
{
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
    return nullptr;
  }

  // User specified a logging implementation, attempt to load that one and only that one
  if (!env_var.empty()) {
    return attempt_to_load_one_logging_library(env_var);
  }

  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "RCL_LOGGING_IMPLEMENTATION not set, using default: %s",
    STRINGIFY(DEFAULT_RCL_LOGGING_IMPLEMENTATION));

  // User didn't specify, so load the default logging implementation
  std::shared_ptr<rcpputils::SharedLibrary> ret;

  ret = attempt_to_load_one_logging_library(STRINGIFY(DEFAULT_RCL_LOGGING_IMPLEMENTATION));
  if (ret != nullptr) {
    return ret;
  }

  // If we made it here, we couldn't find a logging library to load.
  RCUTILS_SET_ERROR_MSG("failed to load any logging implementations");

  return nullptr;
}

std::shared_ptr<rcpputils::SharedLibrary>
get_logging_library()
{
  if (!g_logging_lib) {
    g_logging_lib = load_logging_library();
  }
  return g_logging_lib;
}

void *
lookup_logging_symbol(
  std::shared_ptr<rcpputils::SharedLibrary> lib,
  const std::string & symbol_name)
{
  if (!lib) {
    if (!rcutils_error_is_set()) {
      RCUTILS_SET_ERROR_MSG("no shared library to lookup");
    }  // else assume library loading failed
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

void *
get_logging_symbol(const char * symbol_name)
{
  try {
    return lookup_logging_symbol(get_logging_library(), symbol_name);
  } catch (const std::exception & e) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to get symbol '%s' due to %s",
      symbol_name, e.what());
    return nullptr;
  }
}

#ifdef __cplusplus
extern "C"
{
#endif

#define CALL_SYMBOL(symbol_name, ReturnType, error_value, ...) \
  if (!symbol_ ## symbol_name) { \
    symbol_ ## symbol_name = get_logging_symbol(#symbol_name); \
  } \
  if (!symbol_ ## symbol_name) { \
    /* error message set by get_logging_symbol() */ \
    return error_value; \
  } \
  typedef ReturnType (* FunctionSignature)(__VA_ARGS__); \
  FunctionSignature func = reinterpret_cast<FunctionSignature>(symbol_ ## symbol_name); \
  return func

// Symbol pointers for lazy loading
void * symbol_rcl_logging_external_initialize = nullptr;
void * symbol_rcl_logging_external_shutdown = nullptr;
void * symbol_rcl_logging_external_log = nullptr;
void * symbol_rcl_logging_external_set_logger_level = nullptr;

rcl_logging_ret_t
rcl_logging_external_initialize(
  const char * file_name_prefix,
  const char * config_file,
  rcutils_allocator_t allocator)
{
  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "rcl_logging_external_initialize called (prefix: %s, config: %s)",
    file_name_prefix ? file_name_prefix : "NULL", config_file ? config_file : "NULL");
  CALL_SYMBOL(
    rcl_logging_external_initialize,
    rcl_logging_ret_t,
    RCL_LOGGING_RET_ERROR,
    const char *, const char *, rcutils_allocator_t)(file_name_prefix, config_file, allocator);
}

rcl_logging_ret_t
rcl_logging_external_shutdown(void)
{
  RCUTILS_LOG_DEBUG_NAMED(
    "rcl_logging_implementation",
    "rcl_logging_external_shutdown called");
  if (!symbol_rcl_logging_external_shutdown) {
    symbol_rcl_logging_external_shutdown = get_logging_symbol("rcl_logging_external_shutdown");
  }
  if (!symbol_rcl_logging_external_shutdown) {
    // If shutdown is called before init, it's OK to just return success
    return RCL_LOGGING_RET_OK;
  }

  typedef rcl_logging_ret_t (* FunctionSignature)(void);
  FunctionSignature func = reinterpret_cast<FunctionSignature>(
    symbol_rcl_logging_external_shutdown);
  rcl_logging_ret_t ret = func();

  // Unload the library after successful shutdown
  unload_logging_library();

  return ret;
}

void
rcl_logging_external_log(int severity, const char * name, const char * msg)
{
  if (!symbol_rcl_logging_external_log) {
    symbol_rcl_logging_external_log = get_logging_symbol("rcl_logging_external_log");
  }
  if (!symbol_rcl_logging_external_log) {
    // If log is called before init, just return silently
    RCUTILS_LOG_DEBUG_NAMED(
      "rcl_logging_implementation",
      "rcl_logging_external_log called before init; message dropped (name: %s, msg: %s)",
      name ? name : "NULL", msg ? msg : "NULL");
    return;
  }

  typedef void (* FunctionSignature)(int, const char *, const char *);
  FunctionSignature func = reinterpret_cast<FunctionSignature>(symbol_rcl_logging_external_log);
  func(severity, name, msg);
}

rcl_logging_ret_t
rcl_logging_external_set_logger_level(const char * name, int level)
{
  CALL_SYMBOL(
    rcl_logging_external_set_logger_level,
    rcl_logging_ret_t,
    RCL_LOGGING_RET_ERROR,
    const char *, int)(name, level);
}

#ifdef __cplusplus
}
#endif

void
unload_logging_library()
{
  RCUTILS_LOG_DEBUG_NAMED("rcl_logging_implementation", "Unloading logging library");
  symbol_rcl_logging_external_initialize = nullptr;
  symbol_rcl_logging_external_shutdown = nullptr;
  symbol_rcl_logging_external_log = nullptr;
  symbol_rcl_logging_external_set_logger_level = nullptr;
  g_logging_lib.reset();
}
