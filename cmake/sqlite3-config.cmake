# - Try to find libsqlite3
#
# Once done this will define
#  SQLite3_FOUND - System has libsqlite3
#  SQLite3_INCLUDE_DIRS - The libsqlite3 include directories
#  SQLite3_LIBRARIES - The libraries needed to use libsqlite3

find_path(SQLite3_INCLUDE_PATH sqlite3.h /usr/include/)
if(SQLite3_INCLUDE_PATH)
  set(SQLite3_INCLUDE_DIRS
    ${SQLite3_INCLUDE_PATH}
  )
else()
  message ( "Include path for libsqlite3 could not be determined!")
endif(SQLite3_INCLUDE_PATH)

find_library(SQLite3_LIBRARIES sqlite3)

if (SQLite3_LIBRARIES)
else (SQLite3_LIBRARIES)
   message( "Library of sqlite3 was not found!")
endif (SQLite3_LIBRARIES)


if (SQLite3_INCLUDE_DIRS AND SQLite3_LIBRARIES)
   set(SQLite3_FOUND TRUE)
else (SQLite3_INCLUDE_DIRS AND SQLite3_LIBRARIES)
   set(SQLite3_FOUND FALSE)
endif (SQLite3_INCLUDE_DIRS AND SQLite3_LIBRARIES)

if (SQLite3_FIND_REQUIRED)
  if (SQLite3_FOUND)
  else (SQLite3_FOUND)
    message(FATAL_ERROR "Could not find libsqlite3!")
  endif(SQLite3_FOUND)
endif(SQLite3_FIND_REQUIRED)

mark_as_advanced(SQLite3_LIBRARIES SQLite3_INCLUDE_DIRS)
