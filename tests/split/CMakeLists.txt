# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 2.8)

# test binary for CSV line splitting
project(test_split)

set(test_split_sources
    ../../src/util/Strings.cpp
    split.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O2 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(test_split ${test_split_sources})


# add test for line splitting
add_test(NAME test_split_line
         COMMAND $<TARGET_FILE:test_split>)
