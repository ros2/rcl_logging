cmake_minimum_required(VERSION 3.20)

project(rcl_logging_implementation)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(rcl_logging_interface REQUIRED)
find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)

# This package only provides dynamic loading capabilities.
# For static linking, rcl should link directly to the specific implementation.
message(STATUS "Building rcl_logging_implementation for dynamic loading")

# Set default logging implementation
# Priority: CMake argument (-D) > Environment variable at build time > Default
if(DEFINED ENV{DEFAULT_RCL_LOGGING_IMPLEMENTATION})
  set(DEFAULT_RCL_LOGGING_IMPLEMENTATION "$ENV{DEFAULT_RCL_LOGGING_IMPLEMENTATION}" CACHE STRING
      "Default logging implementation to load if RCL_LOGGING_IMPLEMENTATION environment variable is not set at runtime")
else()
  set(DEFAULT_RCL_LOGGING_IMPLEMENTATION "rcl_logging_spdlog" CACHE STRING
      "Default logging implementation to load if RCL_LOGGING_IMPLEMENTATION environment variable is not set at runtime")
endif()
message(STATUS "Default logging implementation: ${DEFAULT_RCL_LOGGING_IMPLEMENTATION}")

# Create the library
set(${PROJECT_NAME}_sources
  src/functions.cpp
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  "DEFAULT_RCL_LOGGING_IMPLEMENTATION=${DEFAULT_RCL_LOGGING_IMPLEMENTATION}")

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE
  "RCL_LOGGING_IMPLEMENTATION_BUILDING_DLL"
  "RCL_LOGGING_INTERFACE_BUILDING_DLL")

target_link_libraries(${PROJECT_NAME} PUBLIC
  rcl_logging_interface::rcl_logging_interface
  rcutils::rcutils)

target_link_libraries(${PROJECT_NAME} PRIVATE
  rcpputils::rcpputils)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(rcl_logging_interface rcutils)

if(BUILD_TESTING)
  # Causes symbols to be exposed for tests to use.
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    "RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY=RCL_LOGGING_IMPLEMENTATION_PUBLIC")

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(rcl_logging_noop REQUIRED)
  find_package(rcl_logging_spdlog REQUIRED)

  ament_add_gtest(test_logging_implementation test/test_logging_implementation.cpp)
  if(TARGET test_logging_implementation)
    target_link_libraries(test_logging_implementation
      ${PROJECT_NAME}
      rcpputils::rcpputils
      rcutils::rcutils)
  endif()
endif()

ament_package()
