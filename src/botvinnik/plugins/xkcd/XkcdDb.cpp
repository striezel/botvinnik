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

#include "XkcdDb.hpp"
#include <filesystem>
#include <iostream>
#include "../../../util/Directories.hpp"

namespace bvn::XkcdDb
{

std::optional<sql::database> getDatabase()
{
  std::string home;
  std::string dbFileName = "/tmp/xkcd.db";
  if (filesystem::getHome(home))
  {
    const auto delim = filesystem::pathDelimiter;
    const std::string directory = home + delim + ".bvn";
    dbFileName = directory + delim + std::string("xkcd.db");
    try
    {
      // Attempt to create directory and set permissions.
      std::filesystem::create_directories(directory);
      std::filesystem::permissions(directory,
          std::filesystem::perms::owner_all |
          std::filesystem::perms::group_read |
          std::filesystem::perms::others_read);
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: Could not create directory " << directory
                << " and set permissions for it!\nException message: "
                << ex.what() << std::endl;
    }
  }
  const bool needsCreate = !std::filesystem::exists(dbFileName);
  auto db = sql::open(dbFileName);
  if (!db)
  {
    std::clog << "Warning: Failed to open or create database " << dbFileName << std::endl;
    return std::optional<sql::database>();
  }
  if (!needsCreate || createDbStructure(db))
    return db;
  else
    return std::optional<sql::database>();
}

bool createDbStructure(sql::database& db)
{
  const std::string statement = R"SQL(
        CREATE TABLE xkcd (
          comicId INTEGER PRIMARY KEY NOT NULL,
          mxcUri TEXT NOT NULL
        );
        )SQL";
  if (!sql::exec(db, statement))
  {
    std::cerr << "Error: Could not create table in sqlite3 database for xkcd comics!" << std::endl;
    return false;
  }

  return true;
}

std::optional<std::string> getMxcUri(sql::database& db, const unsigned int num)
{
  sql::statement stmt = sql::prepare(db, "SELECT mxcUri FROM xkcd WHERE comicId=@id LIMIT 1;");
  if (!stmt)
  {
    std::cerr << "Error: Failed to prepare select statement for comid id!" << std::endl;
    return std::optional<std::string>();
  }
  if (!sql::bind(stmt, 1, num))
  {
    std::cerr << "Error: Could not bind value of comic Id to prepared statement!" << std::endl;
    return std::optional<std::string>();
  }
  const auto rc = sqlite3_step(stmt.get());
  if (rc == SQLITE_ROW)
    return std::optional<std::string>(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0)));
  if (rc == SQLITE_DONE)
    // No match found!
    return std::optional<std::string>();
  // Some other unexpected error code occurred!
  std::cerr << "Error: Could not get result from xkcd database!" << std::endl
            << sqlite3_errmsg(db.get()) << std::endl;
  return std::optional<std::string>();
}

bool insertMxcUri(sql::database& db, const unsigned int num, const std::string& mxcUri)
{
  auto insert = sql::prepare(db, "INSERT INTO xkcd (comicId, mxcUri) VALUES (@cid, @uri);");
  if (!insert)
  {
    std::cerr << "Error: Could not prepare insert statement for MXC URI!" << std::endl;
    return false;
  }
  if (!sql::bind(insert, 1, num) || !sql::bind(insert, 2, mxcUri))
  {
    std::cerr << "Error: Could not bind values to prepared statement!" << std::endl;
    return false;
  }
  const auto ret = sqlite3_step(insert.get());
  if ((ret != SQLITE_OK) && (ret != SQLITE_DONE))
  {
    std::cerr << "Error: Could not insert URI data for comic #" << num
              << " into database!" << std::endl;
    return false;
  }
  else
    return true;
}

} // namespace
