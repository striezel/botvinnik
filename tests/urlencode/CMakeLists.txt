cmake_minimum_required (VERSION 3.8...3.31)

# test binary for URL-encoding
project(test_urlencode)

set(test_urlencode_sources
    ../../src/net/url_encode.cpp
    urlencode.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE OR ENABLE_SANITIZER)
        add_definitions (-O0)
    else ()
        add_definitions (-O2)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_urlencode ${test_urlencode_sources})

# find cURL library
find_package(PkgConfig)
pkg_search_module (CURL REQUIRED libcurl)
if (CURL_FOUND)
  if (NOT ENABLE_STATIC_LINKING)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (test_urlencode ${CURL_LIBRARIES})
  else ()
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (test_urlencode ${CURL_STATIC_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(test_urlencode Catch2::Catch2WithMain)
endif ()


# add test for urlencoding
add_test(NAME test_urlencode_with_curl
         COMMAND $<TARGET_FILE:test_urlencode>)
