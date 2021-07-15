# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(plugin_tests)

set(plugin_tests_sources
    ../../src/botvinnik/Bot.cpp
    ../../src/botvinnik/FailCounter.cpp
    ../../src/botvinnik/plugins/core/Basic.cpp
    ../../src/botvinnik/plugins/core/Help.cpp
    ../../src/botvinnik/plugins/core/Rooms.cpp
    ../../src/botvinnik/plugins/convert/Conversion.cpp
    ../../src/botvinnik/plugins/corona/Corona.cpp
    ../../src/botvinnik/plugins/corona/CovidNumbers.cpp
    ../../src/botvinnik/plugins/corona/World.cpp
    ../../src/botvinnik/plugins/corona/disease.sh.cpp
    ../../src/botvinnik/plugins/CheatSheet.cpp
    ../../src/botvinnik/plugins/Debian.cpp
    ../../src/botvinnik/plugins/Fortune.cpp
    ../../src/botvinnik/plugins/Ping.cpp
    ../../src/botvinnik/plugins/Wikipedia.cpp
    ../../src/botvinnik/plugins/xkcd/Xkcd.cpp
    ../../src/botvinnik/plugins/xkcd/XkcdData.cpp
    ../../src/botvinnik/plugins/xkcd/XkcdDb.cpp
    ../../src/conf/Configuration.cpp
    ../../src/matrix/Matrix.cpp
    ../../src/matrix/Message.cpp
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/matrix/json/PowerLevels.cpp
    ../../src/matrix/json/Sync.cpp
    ../../src/net/Curly.cpp
    ../../src/net/url_encode.cpp
    ../../src/util/Directories.cpp
    ../../src/util/GitInfos.cpp
    ../../src/util/Strings.cpp
    ../../src/util/chrono.cpp
    ../../src/util/sqlite3.cpp
    ../../third-party/simdjson/simdjson.cpp
    core/Basic.cpp
    core/Help.cpp
    core/Rooms.cpp
    CheatSheet.cpp
    Conversion.cpp
    Corona.cpp
    Debian.cpp
    Fortune.cpp
    Ping.cpp
    Wikipedia.cpp
    Xkcd.cpp
    XkcdData.cpp
    pluginRegistrationCombined.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -O3 -fexceptions)
endif ()

if (USER_AGENT)
  add_definitions (-DBVN_USER_AGENT)
endif(USER_AGENT)

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

# find sqlite3 library
if (CMAKE_VERSION VERSION_LESS "3.14.0")
  # Find module for sqlite3 was added in CMake 3.14.0, so any earlier version
  # needs an extra configuration file to find it.
  set(SQLite3_DIR "../../cmake/" )
endif ()
find_package (SQLite3)
if (SQLite3_FOUND)
  include_directories(${SQLite3_INCLUDE_DIRS})
  target_link_libraries (plugin_tests ${SQLite3_LIBRARIES})
else ()
  message ( FATAL_ERROR "SQLite3 was not found!" )
endif (SQLite3_FOUND)

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
