cmake_minimum_required(VERSION 3.5)

project(rcl_logging_rcutils)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(rcl_logging_interface REQUIRED)
find_package(rcutils REQUIRED)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


add_library(rcl_logging_rcutils
  src/rcl_logging_rcutils.c)

target_link_libraries(rcl_logging_rcutils PRIVATE
  rcutils::rcutils)
target_link_libraries(rcl_logging_rcutils PUBLIC
  rcl_logging_interface::rcl_logging_interface)

target_compile_definitions(rcl_logging_rcutils PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  # TODO(sloretz) tests
endif()

install(TARGETS rcl_logging_rcutils EXPORT export_rcl_logging_rcutils
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)


# Export rcl_logging_interface to give downstream packages access to it's headers
ament_export_dependencies(rcl_logging_interface)
# Generated exported target might say to link to rcutils, so export just in case.
ament_export_dependencies(rcutils)

ament_export_targets(export_rcl_logging_rcutils)

ament_package()
