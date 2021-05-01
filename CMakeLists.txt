# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(botvinnik-matrix-bot)

enable_testing()

# If the option USER_AGENT is enabled (e. g. via `cmake -DUSER_AGENT=ON`)
# during the build, then the bot will send its user agent with every request
# to the Matrix server.
# It is disabled by default, because sending a User-Agent will introduce a
# slight overhead of ca. 30 bytes per request.
option(USER_AGENT "Send user agent to Matrix server" OFF)

# Recurse into subdirectory for the main executable.
add_subdirectory (src)

# Recurse into subdirectory for tests.
add_subdirectory (tests)
