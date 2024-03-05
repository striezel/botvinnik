cmake_minimum_required (VERSION 3.8)

# test binary for Curly PUT request with predefined body
project(test_curly_put_body)

set(test_curly_put_body_sources
    ../../../src/net/Curly.cpp
    put-body.cpp)

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

add_executable(test_curly_put_body ${test_curly_put_body_sources})

# find cURL library
find_package(PkgConfig)
pkg_search_module (CURL REQUIRED libcurl)
if (CURL_FOUND)
  if (NOT ENABLE_STATIC_LINKING)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (test_curly_put_body ${CURL_LIBRARIES})
  else ()
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (test_curly_put_body ${CURL_STATIC_LIBRARIES})
    # cares
    pkg_search_module (CARES REQUIRED libcares)
    if (NOT CARES_FOUND)
      message ( FATAL_ERROR "libcares was not found!" )
    endif ()
    include_directories(${CARES_STATIC_INCLUDE_DIRS})
    target_link_libraries (test_curly_put_body ${CARES_STATIC_LIBRARIES})
    # idn2
    pkg_search_module (IDN2 REQUIRED libidn2)
    if (NOT IDN2_FOUND)
      message ( FATAL_ERROR "libidn2 was not found!" )
    endif ()
    include_directories(${IDN2_STATIC_INCLUDE_DIRS})
    target_link_libraries (test_curly_put_body ${IDN2_STATIC_LIBRARIES})
    # brotlidec
    pkg_search_module (BROTLIDEC REQUIRED libbrotlidec)
    if (NOT BROTLIDEC_FOUND)
      message ( FATAL_ERROR "libbrotlidec was not found!" )
    endif ()
    include_directories(${BROTLIDEC_STATIC_INCLUDE_DIRS})
    target_link_libraries (test_curly_put_body ${BROTLIDEC_STATIC_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)


# add test for Curly class
add_test(NAME Curly_PUT_with_predefined_body
         COMMAND $<TARGET_FILE:test_curly_put_body>)
