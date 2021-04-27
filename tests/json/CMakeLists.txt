# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# test binary for JSON parsing
project(json_tests)

set(json_tests_sources
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/matrix/json/PowerLevels.cpp
    ../../src/matrix/json/Sync.cpp
    ../../third-party/simdjson/simdjson.cpp
    PowerLevels.cpp
    Sync.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -O2 -fexceptions)
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(json_tests ${json_tests_sources})


# add test for JSON parsing
add_test(NAME test_json_parsing
         COMMAND $<TARGET_FILE:json_tests>)
