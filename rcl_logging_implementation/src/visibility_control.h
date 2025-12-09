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

#ifndef VISIBILITY_CONTROL_H_
#define VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define RCL_LOGGING_IMPLEMENTATION_EXPORT __attribute__ ((dllexport))
    #define RCL_LOGGING_IMPLEMENTATION_IMPORT __attribute__ ((dllimport))
  #else
    #define RCL_LOGGING_IMPLEMENTATION_EXPORT __declspec(dllexport)
    #define RCL_LOGGING_IMPLEMENTATION_IMPORT __declspec(dllimport)
  #endif
  #ifdef RCL_LOGGING_IMPLEMENTATION_BUILDING_DLL
    #define RCL_LOGGING_IMPLEMENTATION_PUBLIC RCL_LOGGING_IMPLEMENTATION_EXPORT
  #else
    #define RCL_LOGGING_IMPLEMENTATION_PUBLIC RCL_LOGGING_IMPLEMENTATION_IMPORT
  #endif
  #define RCL_LOGGING_IMPLEMENTATION_PUBLIC_TYPE RCL_LOGGING_IMPLEMENTATION_PUBLIC
  #define RCL_LOGGING_IMPLEMENTATION_LOCAL
#else
  #define RCL_LOGGING_IMPLEMENTATION_EXPORT __attribute__ ((visibility("default")))
  #define RCL_LOGGING_IMPLEMENTATION_IMPORT
  #if __GNUC__ >= 4
    #define RCL_LOGGING_IMPLEMENTATION_PUBLIC __attribute__ ((visibility("default")))
    #define RCL_LOGGING_IMPLEMENTATION_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define RCL_LOGGING_IMPLEMENTATION_PUBLIC
    #define RCL_LOGGING_IMPLEMENTATION_LOCAL
  #endif
  #define RCL_LOGGING_IMPLEMENTATION_PUBLIC_TYPE
#endif

#ifndef RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY
#define RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY
#endif

#ifdef __cplusplus
}
#endif

#endif  // VISIBILITY_CONTROL_H_
