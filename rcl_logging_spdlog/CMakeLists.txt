cmake_minimum_required(VERSION 3.5)

project(rcl_logging_spdlog)

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
find_package(spdlog_vendor REQUIRED) # Provides spdlog on platforms without it.
find_package(spdlog REQUIRED)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(${PROJECT_NAME} src/rcl_logging_spdlog.cpp)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

ament_target_dependencies(${PROJECT_NAME}
  rcl_logging_interface
  rcutils
  spdlog
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(rcpputils REQUIRED)
  ament_add_gtest(test_logging_interface test/test_logging_interface.cpp)
  if(TARGET test_logging_interface)
    target_link_libraries(test_logging_interface ${PROJECT_NAME})
    ament_target_dependencies(test_logging_interface rcpputils)
  endif()
endif()

ament_export_dependencies(ament_cmake rcl_logging_interface rcutils spdlog_vendor spdlog)
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME})
ament_package()
