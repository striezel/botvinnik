/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#ifndef BVN_UTIL_SQLITE3_HPP
#define BVN_UTIL_SQLITE3_HPP

#include <functional>
#include <memory>
#include <sqlite3.h>

namespace bvn::sql
{

// NOTE: database and statement should probably be transformed into classes later.

/// alias for database handle
typedef std::unique_ptr<sqlite3, decltype(&sqlite3_close)> database;

/// alias for database statement handle
typedef std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> statement;

typedef std::function<bool(sqlite3_stmt*)> callback;

/** \brief Opens a new SQLite 3 database connection.
 *
 * If a file with the given name already exists, it will be opened.
 * If no such file exists, it will be created.
 * \param fileName   name of the database file to open / create
 * \return Returns database connection.
 *         If no database could be opened / created, the unique_ptr is not set.
 */
database open(const std::string& fileName);


/** \brief Creates a prepared statement.
 *
 * \param db   open database connection
 * \param sqlStmt   SQL for the prepared statement
 * \return Returns the prepared statement.
 *         If no statement could be prepared, the unique_ptr is not set.
 */
statement prepare(database& db, const std::string& sqlStmt);

bool bind(statement& stmt, const int index, const std::string& value);

bool bind(statement& stmt, const int index, const int64_t value);

}

#endif // BVN_UTIL_SQLITE3_HPP
