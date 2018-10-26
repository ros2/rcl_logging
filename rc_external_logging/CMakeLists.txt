cmake_minimum_required(VERSION 3.5)

project(rc_external_logging)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(rc_external_logging_interface REQUIRED)

include_directories(${ament_INCLUDE_DIRS})
# include_directories(include)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(${PROJECT_NAME}_sources
    src/rc_external_logging/rc_external_logging.cpp
)
#set_source_files_properties(${${PROJECT_NAME}_sources} PROPERTIES language "CXX")

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})
ament_target_dependencies(${PROJECT_NAME} "rc_external_logging_interface")


install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)


ament_export_dependencies(ament_cmake)
#ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_package()

#install(
#  DIRECTORY include/
#  DESTINATION include)
