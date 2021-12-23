# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# test binary for JSON parsing
project(component_tests)

set(component_tests_sources
    ../../src/botvinnik/FailCounter.cpp
    ../../src/botvinnik/plugins/corona/CovidNumbers.cpp
    ../../src/matrix/events/PowerLevels.cpp
    ../../src/net/htmlspecialchars.cpp
    FailCounter.cpp
    PowerLevels.cpp
    calculate_incidence.cpp
    htmlspecialchars.cpp
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

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(component_tests ${component_tests_sources})


# add test case
add_test(NAME test_components
         COMMAND $<TARGET_FILE:component_tests>)
