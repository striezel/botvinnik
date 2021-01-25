# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 2.8)

# test binary for JSON parsing
project(component_tests)

set(component_tests_sources
    ../../src/botvinnik/FailCounter.cpp
    ../../src/botvinnik/plugins/corona/CovidNumbers.cpp
    ../../src/matrix/events/PowerLevels.cpp
    FailCounter.cpp
    PowerLevels.cpp
    calculate_incidence.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O2 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(component_tests ${component_tests_sources})


# add test case
add_test(NAME test_components
         COMMAND $<TARGET_FILE:component_tests>)
