# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# Recurse into subdirectory for executable tests.
add_subdirectory (botvinnik)

# Recurse into subdirectory for component tests.
add_subdirectory (components)

# Recurse into subdirectory for libcurl-related tests.
add_subdirectory (curly)

# Recurse into subdirectory for disease.sh tests.
add_subdirectory (disease.sh)

# Recurse into subdirectory for JSON parsing tests.
add_subdirectory (json)

# Recurse into subdirectory for plugin tests.
add_subdirectory (plugins)

# Recurse into subdirectory for line splitting tests.
add_subdirectory (split)

# Recurse into subdirectory for URL-encoding tests.
add_subdirectory (urlencode)
