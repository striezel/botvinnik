cmake_minimum_required (VERSION 3.8...3.31)

project(botvinnik)

set(botvinnik_sources
    ../../third-party/simdjson/simdjson.cpp
    ../conf/Configuration.cpp
    ../matrix/ImageInfo.cpp
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
    plugins/CheatSheet.cpp
    plugins/DeactivatablePlugin.cpp
    plugins/Debian.cpp
    plugins/Fortune.cpp
    plugins/Giphy.cpp
    plugins/LibreTranslate.cpp
    plugins/Ping.cpp
    plugins/weather/CurrentData.cpp
    plugins/weather/ForecastData.cpp
    plugins/weather/FreeFunctions.cpp
    plugins/weather/Location.cpp
    plugins/weather/LocationLookup.cpp
    plugins/weather/LocationLookupOpenMeteo.cpp
    plugins/weather/LocationLookupOpenStreetMap.cpp
    plugins/weather/OpenMeteo.cpp
    plugins/weather/Weather.cpp
    plugins/weather/WeatherData.cpp
    plugins/Wikipedia.cpp
    plugins/xkcd/Xkcd.cpp
    plugins/xkcd/XkcdData.cpp
    plugins/xkcd/XkcdDb.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -Wshadow -Wpedantic -fexceptions)
    if (CODE_COVERAGE OR ENABLE_SANITIZER)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
        set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
    endif ()
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USER_AGENT)
  add_definitions (-DBVN_USER_AGENT)
endif(USER_AGENT)

add_executable(botvinnik ${botvinnik_sources})

# find cURL library
find_package(PkgConfig)
pkg_search_module (CURL REQUIRED libcurl)
if (CURL_FOUND)
  if (NOT ENABLE_STATIC_LINKING)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${CURL_LIBRARIES})
  else ()
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${CURL_STATIC_LIBRARIES})
    # cares
    pkg_search_module (CARES REQUIRED libcares)
    if (NOT CARES_FOUND)
      message ( FATAL_ERROR "libcares was not found!" )
    endif ()
    include_directories(${CARES_STATIC_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${CARES_STATIC_LIBRARIES})
    # idn2
    pkg_search_module (IDN2 REQUIRED libidn2)
    if (NOT IDN2_FOUND)
      message ( FATAL_ERROR "libidn2 was not found!" )
    endif ()
    include_directories(${IDN2_STATIC_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${IDN2_STATIC_LIBRARIES})
    # brotlidec
    pkg_search_module (BROTLIDEC REQUIRED libbrotlidec)
    if (NOT BROTLIDEC_FOUND)
      message ( FATAL_ERROR "libbrotlidec was not found!" )
    endif ()
    include_directories(${BROTLIDEC_STATIC_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${BROTLIDEC_STATIC_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find sqlite3 library
pkg_search_module (SQLite3 REQUIRED sqlite3)
if (SQLite3_FOUND)
  if (ENABLE_STATIC_LINKING)
    include_directories(${SQLite3_STATIC_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${SQLite3_STATIC_LIBRARIES})
  else ()
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries (botvinnik ${SQLite3_LIBRARIES})
  endif ()
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
