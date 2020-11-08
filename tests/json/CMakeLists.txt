# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 2.8)

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

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O2 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(json_tests ${json_tests_sources})


# add test for JSON parsing
add_test(NAME test_json_parsing
         COMMAND $<TARGET_FILE:json_tests>)
