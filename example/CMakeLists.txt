#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
find_program(MPH_MEMORYCHECK_COMMAND valgrind)

function(example file target)
  add_executable(mph_${target} ${file}.cpp)
  if(MPH_ENABLE_MEMCHECK AND MPH_MEMORYCHECK_COMMAND)
    mph_add_custom_command_or_test(
      TARGET
      mph_${target}
      COMMAND
      ${MPH_MEMORYCHECK_COMMAND}
      --leak-check=full
      --error-exitcode=1
      ./mph_${target}
      ${ARGN}
    )
  else()
    mph_add_custom_command_or_test(TARGET mph_${target} COMMAND mph_${target} ${ARGN})
  endif()
endfunction()

example(hello_world hello_wolrd)
example(dispatch dispatch)
example(policies policies)
