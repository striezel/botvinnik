cmake_minimum_required (VERSION 3.8...3.31)

include (CheckIncludeFileCXX)
check_include_file_cxx (catch2/catch_test_macros.hpp HAS_CATCH_V3_HEADER)
check_include_file_cxx (catch2/catch.hpp HAS_CATCH_V2_HEADER)

if (HAS_CATCH_V3_HEADER AND NOT HAS_CATCH_V2_HEADER)
  set(HAS_CATCH_V3 ON)
else ()
  set(HAS_CATCH_V3 OFF)
endif ()

# Recurse into subdirectory for executable tests.
add_subdirectory (botvinnik)

# Recurse into subdirectory for component tests.
add_subdirectory (components)

# Recurse into subdirectory for libcurl-related tests.
add_subdirectory (curly)

# Recurse into subdirectory for JSON parsing tests.
add_subdirectory (json)

# Recurse into subdirectory for Matrix-related tests.
# These tests require a local Matrix homeserver.
add_subdirectory (matrix)

# Recurse into subdirectory for plugin tests.
add_subdirectory (plugins)

# Recurse into subdirectory for URL-encoding tests.
add_subdirectory (urlencode)
