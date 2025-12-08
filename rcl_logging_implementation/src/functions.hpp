// Copyright 2024 Open Source Robotics Foundation, Inc.
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

#ifndef RCL_LOGGING_IMPLEMENTATION__FUNCTIONS_HPP_
#define RCL_LOGGING_IMPLEMENTATION__FUNCTIONS_HPP_

#include <memory>
#include <string>

#include "rcpputils/shared_library.hpp"

std::shared_ptr<rcpputils::SharedLibrary> load_logging_library();

std::shared_ptr<rcpputils::SharedLibrary> get_logging_library();

void * lookup_logging_symbol(
  std::shared_ptr<rcpputils::SharedLibrary> lib,
  const std::string & symbol_name);

void * get_logging_symbol(const char * symbol_name);

void unload_logging_library();

#endif  // RCL_LOGGING_IMPLEMENTATION__FUNCTIONS_HPP_
