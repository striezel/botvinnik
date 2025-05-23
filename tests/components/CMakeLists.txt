cmake_minimum_required (VERSION 3.8...3.31)

# test binary for component tests
project(component_tests)

set(component_tests_sources
    ../../src/botvinnik/FailCounter.cpp
    ../../src/botvinnik/plugins/corona/CovidNumbers.cpp
    ../../src/conf/Configuration.cpp
    ../../src/matrix/ImageInfo.cpp
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/net/htmlspecialchars.cpp
    ../../src/util/Directories.cpp
    ../../src/util/sqlite3.cpp
    ../../src/util/Strings.cpp
    botvinnik/FailCounter.cpp
    conf/Configuration.cpp
    matrix/ImageInfo.cpp
    matrix/events/PowerLevels.cpp
    net/htmlspecialchars.cpp
    util/Directories.cpp
    util/sqlite3.cpp
    util/Strings.cpp
    calculate_incidence.cpp
    Version.cpp
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

add_executable(component_tests ${component_tests_sources})

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(component_tests Catch2::Catch2WithMain)
endif ()

# find sqlite3 library
find_package(PkgConfig)
pkg_search_module (SQLite3 REQUIRED sqlite3)
if (SQLite3_FOUND)
  if (ENABLE_STATIC_LINKING)
    include_directories(${SQLite3_STATIC_INCLUDE_DIRS})
    target_link_libraries (component_tests ${SQLite3_STATIC_LIBRARIES})
  else ()
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries (component_tests ${SQLite3_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "SQLite3 was not found!" )
endif (SQLite3_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(component_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
    # If we are on Clang 7.x, then the filesystem library from GCC is better.
    target_link_libraries(component_tests stdc++fs)
  else ()
    # Use Clang's C++ filesystem library, it is recent enough.
    target_link_libraries(component_tests c++fs)
  endif ()
endif ()


# add test case
add_test(NAME test_components
         COMMAND $<TARGET_FILE:component_tests>)
