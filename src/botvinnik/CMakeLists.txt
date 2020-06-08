# Might work with earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(botvinnik)

set(botvinnik_sources
    ../../third-party/simdjson/simdjson.cpp
    ../conf/Configuration.cpp
    ../matrix/Matrix.cpp
    ../matrix/Message.cpp
    ../net/Curly.cpp
    ../net/url_encode.cpp
    ../util/chrono.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/sqlite3.cpp
    ../util/Strings.cpp
    Bot.cpp
    plugins/core/Basic.cpp
    plugins/core/Help.cpp
    plugins/Corona.cpp
    plugins/Debian.cpp
    plugins/Fortune.cpp
    plugins/Ping.cpp
    plugins/Wikipedia.cpp
    plugins/Xkcd.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(botvinnik ${botvinnik_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (botvinnik ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(botvinnik stdc++fs)
endif ()

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

message("GIT_HASH is ${GIT_HASH}.")
message("GIT_TIME is ${GIT_TIME}.")

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.cpp
               ESCAPE_QUOTES)
