cmake_minimum_required (VERSION 3.8)

# MSYS2 / MinGW uses Catch 3.x.
if (MINGW)
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

# Recurse into subdirectory for plugin tests.
add_subdirectory (plugins)

# Recurse into subdirectory for line splitting tests.
add_subdirectory (split)

# Recurse into subdirectory for URL-encoding tests.
add_subdirectory (urlencode)
