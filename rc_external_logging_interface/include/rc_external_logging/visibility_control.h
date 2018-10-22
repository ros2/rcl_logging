// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifndef RC_EXTERNAL_LOGGING__VISIBILITY_CONTROL_H_
#define RC_EXTERNAL_LOGGING__VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define RC_EXTERNAL_LOGGING_EXPORT __attribute__ ((dllexport))
    #define RC_EXTERNAL_LOGGING_IMPORT __attribute__ ((dllimport))
  #else
    #define RC_EXTERNAL_LOGGING_EXPORT __declspec(dllexport)
    #define RC_EXTERNAL_LOGGING_IMPORT __declspec(dllimport)
  #endif
  #ifdef RC_EXTERNAL_LOGGING_BUILDING_DLL
    #define RC_EXTERNAL_LOGGING_PUBLIC RC_EXTERNAL_LOGGING_EXPORT
  #else
    #define RC_EXTERNAL_LOGGING_PUBLIC RC_EXTERNAL_LOGGING_IMPORT
  #endif
  #define RC_EXTERNAL_LOGGING_PUBLIC_TYPE RC_EXTERNAL_LOGGING_PUBLIC
  #define RC_EXTERNAL_LOGGING_LOCAL
#else
  #define RC_EXTERNAL_LOGGING_EXPORT __attribute__ ((visibility("default")))
  #define RC_EXTERNAL_LOGGING_IMPORT
  #if __GNUC__ >= 4
    #define RC_EXTERNAL_LOGGING_PUBLIC __attribute__ ((visibility("default")))
    #define RC_EXTERNAL_LOGGING_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define RC_EXTERNAL_LOGGING_PUBLIC
    #define RC_EXTERNAL_LOGGING_LOCAL
  #endif
  #define RC_EXTERNAL_LOGGING_PUBLIC_TYPE
#endif

#ifdef __cplusplus
}
#endif

#endif  // RC_EXTERNAL_LOGGING__VISIBILITY_CONTROL_H_
