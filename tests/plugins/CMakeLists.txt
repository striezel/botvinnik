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
    ../../src/botvinnik/plugins/CheatSheet.cpp
    ../../src/botvinnik/plugins/DeactivatablePlugin.cpp
    ../../src/botvinnik/plugins/Debian.cpp
    ../../src/botvinnik/plugins/Fortune.cpp
    ../../src/botvinnik/plugins/Giphy.cpp
    ../../src/botvinnik/plugins/LibreTranslate.cpp
    ../../src/botvinnik/plugins/Ping.cpp
    ../../src/botvinnik/plugins/weather/Location.cpp
    ../../src/botvinnik/plugins/weather/LocationLookupOpenStreetMap.cpp
    ../../src/botvinnik/plugins/Wikipedia.cpp
    ../../src/botvinnik/plugins/xkcd/Xkcd.cpp
    ../../src/botvinnik/plugins/xkcd/XkcdData.cpp
    ../../src/botvinnik/plugins/xkcd/XkcdDb.cpp
    ../../src/conf/Configuration.cpp
    ../../src/matrix/ImageInfo.cpp
    ../../src/matrix/Matrix.cpp
    ../../src/matrix/Message.cpp
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/matrix/json/PowerLevels.cpp
    ../../src/matrix/json/Sync.cpp
    ../../src/net/Curly.cpp
    ../../src/net/htmlspecialchars.cpp
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
    CommandDeactivation.cpp
    Conversion.cpp
    Corona.cpp
    Debian.cpp
    Fortune.cpp
    Giphy.cpp
    LibreTranslate.cpp
    Ping.cpp
    weather/Location.cpp
    weather/LocationLookupOpenStreetMap.cpp
    Wikipedia.cpp
    Xkcd.cpp
    XkcdData.cpp
    pluginRegistration.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

if (USER_AGENT)
  add_definitions (-DBVN_USER_AGENT)
endif(USER_AGENT)

add_executable(plugin_tests ${plugin_tests_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  if (NOT ENABLE_STATIC_LINKING)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${CURL_LIBRARIES})
  else ()
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${CURL_STATIC_LIBRARIES})
    # cares
    pkg_search_module (CARES REQUIRED libcares)
    if (NOT CARES_FOUND)
      message ( FATAL_ERROR "libcares was not found!" )
    endif ()
    include_directories(${CARES_STATIC_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${CARES_STATIC_LIBRARIES})
    # idn2
    pkg_search_module (IDN2 REQUIRED libidn2)
    if (NOT IDN2_FOUND)
      message ( FATAL_ERROR "libidn2 was not found!" )
    endif ()
    include_directories(${IDN2_STATIC_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${IDN2_STATIC_LIBRARIES})
    # brotlidec
    pkg_search_module (BROTLIDEC REQUIRED libbrotlidec)
    if (NOT BROTLIDEC_FOUND)
      message ( FATAL_ERROR "libbrotlidec was not found!" )
    endif ()
    include_directories(${BROTLIDEC_STATIC_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${BROTLIDEC_STATIC_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find sqlite3 library
find_package(PkgConfig)
pkg_search_module (SQLite3 REQUIRED sqlite3)
if (SQLite3_FOUND)
  if (ENABLE_STATIC_LINKING)
    include_directories(${SQLite3_STATIC_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${SQLite3_STATIC_LIBRARIES})
  else ()
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries (plugin_tests ${SQLite3_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "SQLite3 was not found!" )
endif (SQLite3_FOUND)

if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(plugin_tests Catch2::Catch2WithMain)
endif ()

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(plugin_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(plugin_tests stdc++fs)
  else ()
    # Use Clang's C++ filesystem library, it is recent enough.
    target_link_libraries(plugin_tests c++fs)
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

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../src/util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../../src/util/GitInfos.cpp
               ESCAPE_QUOTES)

# test run
add_test(NAME plugin-test-suite
         COMMAND $<TARGET_FILE:plugin_tests>)
