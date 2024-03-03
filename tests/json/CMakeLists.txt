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
    add_definitions (-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O2)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(json_tests ${json_tests_sources})

if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(json_tests Catch2::Catch2WithMain)
endif ()


# add test for JSON parsing
add_test(NAME test_json_parsing
         COMMAND $<TARGET_FILE:json_tests>)
