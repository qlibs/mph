#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

if(NOT CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message(FATAL_ERROR "Build type must be set to RELEASE. Aborting build.")
endif()

function(benchmark name file)
  add_executable(b${name} ${file}.cpp)
  target_compile_options(b${name} PRIVATE ${ARGN})
  target_link_libraries(b${name} PRIVATE mph)
  target_include_directories(b${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_options(b${name} PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=100000000>)
  target_compile_options(b${name} PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=100000000>)
  mph_add_custom_command_or_test(TARGET b${name} COMMAND b${name})
endfunction()

benchmark(benchmark benchmark)
