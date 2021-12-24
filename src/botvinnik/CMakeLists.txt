# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(botvinnik)

set(botvinnik_sources
    ../../third-party/simdjson/simdjson.cpp
    ../conf/Configuration.cpp
    ../matrix/Matrix.cpp
    ../matrix/Message.cpp
    ../matrix/events/PowerLevels.cpp
    ../matrix/json/PowerLevels.cpp
    ../matrix/json/Sync.cpp
    ../net/Curly.cpp
    ../net/htmlspecialchars.cpp
    ../net/url_encode.cpp
    ../util/chrono.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/sqlite3.cpp
    ../util/Strings.cpp
    Bot.cpp
    FailCounter.cpp
    plugins/core/Basic.cpp
    plugins/core/Help.cpp
    plugins/core/Rooms.cpp
    plugins/convert/Conversion.cpp
    plugins/corona/Corona.cpp
    plugins/corona/CovidNumbers.cpp
    plugins/corona/World.cpp
    plugins/corona/disease.sh.cpp
    plugins/CheatSheet.cpp
    plugins/Debian.cpp
    plugins/Fortune.cpp
    plugins/Ping.cpp
    plugins/Wikipedia.cpp
    plugins/xkcd/Xkcd.cpp
    plugins/xkcd/XkcdData.cpp
    plugins/xkcd/XkcdDb.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USER_AGENT)
  add_definitions (-DBVN_USER_AGENT)
endif(USER_AGENT)

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

# find sqlite3 library
if (CMAKE_VERSION VERSION_LESS "3.14.0")
  # Find module for sqlite3 was added in CMake 3.14.0, so any earlier version
  # needs an extra configuration file to find it.
  set(SQLite3_DIR "../../cmake/" )
endif ()
find_package (SQLite3)
if (SQLite3_FOUND)
  include_directories(${SQLite3_INCLUDE_DIRS})
  target_link_libraries (botvinnik ${SQLite3_LIBRARIES})
else ()
  message ( FATAL_ERROR "SQLite3 was not found!" )
endif (SQLite3_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(botvinnik stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(botvinnik stdc++fs)
  else ()
   # Use Clang's C++ filesystem library, it is recent enough.
   target_link_libraries(botvinnik c++fs)
  endif ()
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
