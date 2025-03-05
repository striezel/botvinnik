cmake_minimum_required (VERSION 3.8...3.31)

# test binary for Matrix-related tests
project(matrix_tests)

set(matrix_tests_sources
    ../../src/conf/Configuration.cpp
    ../../src/matrix/ImageInfo.cpp
    ../../src/matrix/Matrix.cpp
    ../../src/matrix/Message.cpp
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/matrix/json/PowerLevels.cpp
    ../../src/matrix/json/Sync.cpp
    ../../src/net/Curly.cpp
    ../../src/net/url_encode.cpp
    ../../src/util/Directories.cpp
    ../../src/util/Strings.cpp
    ../../src/util/chrono.cpp
    ../../third-party/simdjson/simdjson.cpp
    Matrix.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE OR ENABLE_SANITIZER)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(matrix_tests ${matrix_tests_sources})

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(matrix_tests Catch2::Catch2WithMain)
endif ()

# find cURL library
find_package(PkgConfig)
pkg_search_module (CURL REQUIRED libcurl)
if (CURL_FOUND)
  if (NOT ENABLE_STATIC_LINKING)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (matrix_tests ${CURL_LIBRARIES})
  else ()
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (matrix_tests ${CURL_STATIC_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(matrix_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(matrix_tests stdc++fs)
  else ()
    # Use Clang's C++ filesystem library, it is recent enough.
    target_link_libraries(matrix_tests c++fs)
  endif ()
endif ()


# add test case
if (NOT DEFINED ENV{GITHUB_ACTIONS})
  message ( STATUS "Skipping Matrix-related tests, because this is no CI environment.")
else ()
  add_test(NAME test_matrix_with_server
           COMMAND $<TARGET_FILE:matrix_tests>)
endif ()
