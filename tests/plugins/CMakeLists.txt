# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 2.8)

project(plugin_tests)

set(plugin_tests_sources
    ../../src/botvinnik/Bot.cpp
    ../../src/botvinnik/plugins/core/Basic.cpp
    ../../src/botvinnik/plugins/core/Help.cpp
    ../../src/conf/Configuration.cpp
    ../../src/matrix/Matrix.cpp
    ../../src/net/Curly.cpp
    ../../src/util/Directories.cpp
    ../../src/util/GitInfos.cpp
    ../../src/util/Strings.cpp
    ../../src/util/chrono.cpp
    ../../third-party/simdjson/simdjson.cpp
    core/Basic.cpp
    core/Help.cpp
    main.cpp)

add_definitions(-std=c++17)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(plugin_tests ${plugin_tests_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (plugin_tests ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(plugin_tests stdc++fs)
endif (NOT MSVC)

# create git-related constants
# -- get the current commit hash
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# -- get the commit date
execute_process(
  COMMAND git show -s --format=%ci
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TIME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../src/util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../../src/util/GitInfos.cpp
               ESCAPE_QUOTES)

# test run
add_test(NAME plugin-test-suite
         COMMAND $<TARGET_FILE:plugin_tests>)
