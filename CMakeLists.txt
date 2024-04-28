#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.21)
project(
  benchmark
  LANGUAGES CXX
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD
      20
      CACHE STRING "Default value for CXX_STANDARD property of targets."
  )
  option(CMAKE_CXX_STANDARD_REQUIRED "Default value for CXX_STANDARD_REQUIRED property of targets." YES)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message(FATAL_ERROR ${CMAKE_BUILD_TYPE} " build type must be set to RELEASE. Aborting build.")
endif()

function(mph_add_custom_command_or_test)
  set(prefix "PARSE")
  set(noValues "")
  set(singleValues TARGET)
  set(multiValues COMMAND)

  include(CMakeParseArguments)
  cmake_parse_arguments("${prefix}" "${noValues}" "${singleValues}" "${multiValues}" ${ARGN})
  add_test(NAME ${PARSE_TARGET} COMMAND ${PARSE_COMMAND})
endfunction()

function(benchmark name file)
  add_executable(${name} ${file}.cpp)
  target_compile_options(${name} PRIVATE ${ARGN})
  target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_options(${name} PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=100000000>)
  target_compile_options(${name} PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=100000000>)
  mph_add_custom_command_or_test(TARGET ${name} COMMAND ${name})
endfunction()

benchmark(benchmark benchmark)
