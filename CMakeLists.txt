cmake_minimum_required(VERSION 3.16)

project(simple_6502_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

function(simple6502_enable_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
  endif()
endfunction()

option(SIMPLE6502_BUILD_INTERACTIVE
       "Build interactive terminal frontends"
       ON)

add_library(emu_core STATIC
  emu_core.cpp
)
target_include_directories(emu_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
simple6502_enable_warnings(emu_core)

add_library(apple2_machine STATIC
  apple2_machine.cpp
)
target_link_libraries(apple2_machine PUBLIC emu_core)
target_include_directories(apple2_machine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
simple6502_enable_warnings(apple2_machine)

add_executable(functional_test
  tests/functional_test.cpp
)
target_link_libraries(functional_test PRIVATE emu_core)
simple6502_enable_warnings(functional_test)

add_executable(apple1_smoke_test
  tests/apple1_smoke_test.cpp
)
target_link_libraries(apple1_smoke_test PRIVATE emu_core)
simple6502_enable_warnings(apple1_smoke_test)

if(SIMPLE6502_BUILD_INTERACTIVE)
  add_executable(emu
    emu_main.cpp
    console.cpp
  )
  target_link_libraries(emu PRIVATE emu_core)
  simple6502_enable_warnings(emu)

  add_executable(apple2_cli
    apple2_cli.cpp
  )
  target_link_libraries(apple2_cli PRIVATE apple2_machine)
  simple6502_enable_warnings(apple2_cli)
endif()

include(CTest)
if(BUILD_TESTING)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/ROM"
       DESTINATION "${CMAKE_BINARY_DIR}")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/build")

  add_test(NAME functional_test
           COMMAND functional_test)
  set_tests_properties(functional_test PROPERTIES
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

  add_test(NAME apple1_smoke_test
           COMMAND apple1_smoke_test)
  set_tests_properties(apple1_smoke_test PROPERTIES
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

  if(TARGET apple2_cli)
    add_test(NAME apple2_cli_smoke
             COMMAND apple2_cli --smoke-test)
    set_tests_properties(apple2_cli_smoke PROPERTIES
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  endif()
endif()

if(WIN32 AND NOT SIMPLE6502_BUILD_INTERACTIVE)
  message(STATUS
    "Skipping interactive frontends; building emu_core, "
    "apple2_machine, functional_test, and apple1_smoke_test.")
endif()
