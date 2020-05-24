# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 2.8)

# Recurse into subdirectory for executable tests.
add_subdirectory (botvinnik)

# Recurse into subdirectory for libcurl-related tests.
add_subdirectory (curly)

# Recurse into subdirectory for plugin tests.
add_subdirectory (plugins)
