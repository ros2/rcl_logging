cmake_minimum_required(VERSION 3.5)

project(rc_external_logging_interface)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake_ros REQUIRED)

include_directories(include)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(${PROJECT_NAME}_sources )
set_source_files_properties(${${PROJECT_NAME}_sources} PROPERTIES language "C")

add_library(${PROJECT_NAME} INTERFACE)


install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)


ament_export_dependencies(ament_cmake)
ament_export_include_directories(include)
#ament_export_libraries(${PROJECT_NAME})
ament_package()

install(
  DIRECTORY include/
  DESTINATION include)
