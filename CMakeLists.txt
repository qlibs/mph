#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.21...3.25)
project(
  mph
  VERSION 1.0.0
  LANGUAGES CXX
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD
      20
      CACHE STRING "Default value for CXX_STANDARD property of targets."
  )
  option(CMAKE_CXX_STANDARD_REQUIRED "Default value for CXX_STANDARD_REQUIRED property of targets." YES)
endif()

option(MPH_ENABLE_MEMCHECK "Run the unit tests and examples under valgrind if it is found" OFF)
option(MPH_ENABLE_SANITIZERS "Run static analysis" OFF)
option(MPH_BUILD_BENCHMARKS "Build the benchmarks" OFF)
option(MPH_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
option(MPH_BUILD_TESTS "Build the tests" ${PROJECT_IS_TOP_LEVEL})
option(MPH_USE_WARNINGS_AS_ERORS "Build the tests" ${PROJECT_IS_TOP_LEVEL})

add_library(mph INTERFACE)

if(NOT DEFINED INCLUDE_INSTALL_DIR)
  set(INCLUDE_INSTALL_DIR include/${PROJECT_NAME}-${PROJECT_VERSION})
endif()
target_include_directories(mph INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>)
target_compile_features(mph INTERFACE cxx_std_20)

if(MPH_USE_WARNINGS_AS_ERORS)
  include(cmake/WarningsAsErrors.cmake)
endif()

add_custom_target(style)
add_custom_command(
  TARGET style COMMAND find ${CMAKE_CURRENT_LIST_DIR}/benchmark ${CMAKE_CURRENT_LIST_DIR}/example ${CMAKE_CURRENT_LIST_DIR}
                       ${CMAKE_CURRENT_LIST_DIR}/test -iname "*.hpp" -or -iname "*.cpp" | xargs clang-format -i
)

if(MPH_ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

if(MPH_ENABLE_SANITIZERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif()

include(cmake/AddCustomCommandOrTest.cmake)

if(MPH_BUILD_BENCHMARKS)
  add_subdirectory(benchmark)
endif()

if(MPH_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

if(MPH_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
