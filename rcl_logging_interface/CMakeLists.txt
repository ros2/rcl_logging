cmake_minimum_required(VERSION 3.20)

project(rcl_logging_interface)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_ros_core REQUIRED)
find_package(rcutils REQUIRED)

set(${PROJECT_NAME}_sources
  "src/logging_dir.c"
)
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_link_libraries(${PROJECT_NAME} PUBLIC rcutils::rcutils)
target_link_libraries(${PROJECT_NAME} PRIVATE ament_cmake_ros_core::ament_ros_defaults)
target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(rcutils)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(rcpputils REQUIRED)
  ament_add_gtest(test_get_logging_directory test/test_get_logging_directory.cpp)
  if(TARGET test_get_logging_directory)
    target_link_libraries(test_get_logging_directory ${PROJECT_NAME} ament_cmake_ros_core::ament_ros_defaults rcpputils::rcpputils rcutils::rcutils)
  endif()
endif()

ament_package()
