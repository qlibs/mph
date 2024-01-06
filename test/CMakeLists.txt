#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
find_program(MPH_MEMORYCHECK_COMMAND valgrind)

if(MPH_ENABLE_MEMCHECK AND MPH_MEMORYCHECK_COMMAND)
  function(ut name)
    add_executable(${name}_test ${name}.cpp)
    target_include_directories(${name}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_options(${name}_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=100000000>)
    target_compile_options(${name}_test PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=100000000>)
    mph_add_custom_command_or_test(
      TARGET
      ${name}_test
      COMMAND
      ${MPH_MEMORYCHECK_COMMAND}
      --leak-check=full
      --error-exitcode=1
      ${CMAKE_CROSSCOMPILING_EMULATOR}
      ./${name}_test
    )
  endfunction()
else()
  function(ut name)
    add_executable(${name}_test ${name}.cpp)
    target_include_directories(${name}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_options(${name}_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=100000000>)
    target_compile_options(${name}_test PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=100000000>)
    mph_add_custom_command_or_test(TARGET ${name}_test COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${name}_test)
  endfunction()
endif()

ut(mph)
