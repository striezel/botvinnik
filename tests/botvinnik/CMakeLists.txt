cmake_minimum_required (VERSION 3.8...3.31)

# ############################## #
# tests for botvinnik executable #
# ############################## #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME botvinnik_help
         COMMAND $<TARGET_FILE:botvinnik> --help)

# short help parameter with question mark "-?"
add_test(NAME botvinnik_help_question_mark
         COMMAND $<TARGET_FILE:botvinnik> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
    add_test(NAME botvinnik_help_question_mark_windows
             COMMAND $<TARGET_FILE:botvinnik> /?)
endif ()

# parameter to show version number
add_test(NAME botvinnik_version
         COMMAND $<TARGET_FILE:botvinnik> --version)
