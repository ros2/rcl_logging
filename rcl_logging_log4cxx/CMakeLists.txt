cmake_minimum_required(VERSION 3.5)

project(rcl_logging_log4cxx)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake_ros REQUIRED)
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Log4cxx REQUIRED)

include_directories(${ament_INCLUDE_DIRS} include)


if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(${PROJECT_NAME}_sources
    src/rcl_logging_log4cxx/rcl_logging_log4cxx.cpp
)
#set_source_files_properties(${${PROJECT_NAME}_sources} PROPERTIES language "CXX")

if(NOT WIN32)
  add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})
else()
  add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_sources})
endif()

ament_target_dependencies(${PROJECT_NAME}
    Log4cxx
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

ament_export_include_directories(include)
ament_export_dependencies(ament_cmake)
ament_export_libraries(${PROJECT_NAME} log4cxx)
ament_package()
