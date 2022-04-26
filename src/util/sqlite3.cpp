/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2022  Dirk Stolle

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

#include "sqlite3.hpp"
#include <iostream>

namespace bvn::sql
{

database open(const std::string& fileName)
{
  sqlite3* dbPtr = nullptr;
  const int errorCode = sqlite3_open(fileName.c_str(), &dbPtr);
  if (errorCode != SQLITE_OK)
  {
    std::cerr << "Error: Could not open database " << fileName << ": "
              << sqlite3_errmsg(dbPtr) << std::endl;
    sqlite3_close(dbPtr);
    return { nullptr, sqlite3_close };
  }

  return { dbPtr, sqlite3_close };
}

bool exec(database& db, const std::string& sql)
{
  char * errorMessage = nullptr;
  if (sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK)
  {
    std::cerr << "Error: Could execute SQL statement in sqlite3 database!" << std::endl
              << errorMessage << std::endl;
    sqlite3_free(errorMessage);
    return false;
  }

  return true;
}

std::string quote(const std::string& str)
{
  std::string result(str);
  std::string::size_type pos = 0;
  std::string::size_type found = std::string::npos;
  while ((found = result.find('\'', pos)) != std::string::npos)
  {
    result.replace(found, 1, "''");
    pos = found + 2;
  }

  return std::string("'").append(result).append("'");
}

statement prepare(database& db, const std::string& sqlStmt)
{
  sqlite3_stmt * stmt = nullptr;
  const int errorCode = sqlite3_prepare_v2(db.get(), sqlStmt.c_str(), sqlStmt.size(), &stmt, nullptr);
  if (errorCode != SQLITE_OK)
  {
    std::cerr << "Error: Could not create prepared statement for '" << sqlStmt << "'!\n"
              << sqlite3_errmsg(db.get()) << std::endl;
    return { nullptr, sqlite3_finalize };
  }

  return { stmt, sqlite3_finalize };
}

bool bind(statement& stmt, const int index, const std::string& value)
{
  return sqlite3_bind_text(stmt.get(), index, value.c_str(), static_cast<int>(value.size()), SQLITE_TRANSIENT) == SQLITE_OK;
}

bool bind(statement& stmt, const int index, const int64_t value)
{
  return sqlite3_bind_int64(stmt.get(), index, value) == SQLITE_OK;
}

} // namespace
