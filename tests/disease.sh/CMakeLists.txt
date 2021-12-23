# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# test binary for disease.sh
project(test_disease_sh)

set(test_disease_sh_sources
    ../../src/botvinnik/plugins/corona/CovidNumbers.cpp
    ../../src/botvinnik/plugins/corona/disease.sh.cpp
    ../../src/net/Curly.cpp
    ../../third-party/simdjson/simdjson.cpp
    disease.sh.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O2)
    endif ()
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(test_disease_sh ${test_disease_sh_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (test_disease_sh ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)


# add test for disease.sh API
add_test(NAME test_disease.sh_api
         COMMAND $<TARGET_FILE:test_disease_sh>)
