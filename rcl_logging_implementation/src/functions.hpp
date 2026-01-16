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

#ifndef FUNCTIONS_HPP_
#define FUNCTIONS_HPP_

#include <memory>
#include <string>

#include "rcpputils/shared_library.hpp"

#include "./visibility_control.h"

RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY
bool load_logging_library();

RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY
void unload_logging_library();

// Test-only function to force unload the library and reset all state
// This allows tests to load different implementations in the same process
RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY
void force_unload_logging_library();

#endif  // FUNCTIONS_HPP_
