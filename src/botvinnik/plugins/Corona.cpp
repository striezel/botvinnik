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

#include "Corona.hpp"
#include <climits>
#include <filesystem>
#include <iostream>
#include <random>
#include <sstream>
#include <sqlite3.h>
#include "../../net/Curly.hpp"
#include "../../util/Directories.hpp"
#include "../../util/sqlite3.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

int64_t getInt64(const std::string& value)
{
  try
  {
    std::size_t pos;
    #if LONG_MAX == 9223372036854775807LL
    // long int is 64 bit.
    const int64_t i = std::stol(value, &pos);
    #elif LLONG_MAX == 9223372036854775807LL
    // long long int is 64 bit.
    const int64_t i = std::stoll(value, &pos);
    #else
      #error Could not find suitable 64-bit integer type!
    #endif // LONG_MAX
    if (pos != value.size())
    {
      std::cerr << "Error: '"<< value << "' is not a valid signed 64-bit integer!";
      return -1;
    }
    return i;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not convert value '" << value << "' to int64_t!" << std::endl;
    return -1;
  }
}

std::string getTempFileName()
{
  namespace fs = std::filesystem;
  const auto dirPath = fs::temp_directory_path();
  const std::string chars("abcdefghijklmopqrstuvwxyz0123456789");
  std::random_device randDev;
  std::mt19937 generator(randDev());
  std::uniform_int_distribution<unsigned int> distribution(0, chars.size() - 1);
  std::string basename;
  do
  {
    basename = "db_";
    for (auto i = 0; i < 16; ++i)
    {
      basename.append(1, chars.at(distribution(generator)));
    }
  } while (fs::exists(dirPath / basename));

  return (dirPath / basename).native();
}

bool createDbStructure(sql::database& db)
{
  const std::string statement = R"SQL(
        CREATE TABLE country (
          countryId INTEGER PRIMARY KEY NOT NULL,
          name TEXT NOT NULL,
          population INTEGER,
          geoId TEXT NOT NULL,
          countryCode TEXT,
          continent TEXT
        );
        CREATE TABLE covid19 (
          countryId INTEGER NOT NULL,
          date TEXT,
          cases INTEGER,
          deaths INTEGER
        );
        )SQL";
  char * errorMessage = nullptr;
  if (sqlite3_exec(db.get(), statement.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK)
  {
    std::cerr << "Error: Could not create tables in sqlite3 database!" << std::endl
              << errorMessage << std::endl;
    sqlite3_free(errorMessage);
    return false;
  }

  return true;
}

int64_t getCountryId(sql::database& db, const std::string& geoId, const std::string& name,
                     const int64_t pop, const std::string& countryCode, const std::string& continent)
{
  sql::statement stmt = sql::prepare(db, "SELECT countryId FROM country WHERE geoId=@id LIMIT 1;");
  if (!stmt)
  {
    std::cerr << "Error: Failed to prepare select statement for geoId!" << std::endl;
    return -1;
  }
  if (!sql::bind(stmt, 1, geoId))
  {
    std::cerr << "Error: Could not bind value of geoId to prepared statement!" << std::endl;
    return -1;
  }

  const auto rc = sqlite3_step(stmt.get());
  if (rc == SQLITE_ROW)
    return sqlite3_column_int64(stmt.get(), 0);
  if (rc == SQLITE_DONE)
  {
    // Not found - insert it.
    auto insert = sql::prepare(db, "INSERT INTO country (name, population, geoId, countryCode, continent) VALUES (@countryname, @pop, @geo, @code, @continent);");
    if (!insert)
    {
      std::cerr << "Error: Could not prepare insert statement for geoId!" << std::endl;
      return -1;
    }
    /* if (!sql::bind(insert, 1, name) || !sql::bind(insert, 2, pop) || !sql::bind(insert, 3, geoId)
        || !sql::bind(insert, 4, countryCode) || !sql::bind(insert, 5, continent))
    {
      std::cerr << "Error: Could not bind values to prepared statement!" << std::endl;
      return -1;
    } */
    if (!sql::bind(insert, 1, name))
    {
      std::cerr << "Error: Could not bind name value to prepared statement!" << std::endl;
      return -1;
    }
    if (!sql::bind(insert, 2, pop))
    {
      std::cerr << "Error: Could not bind population value to prepared statement!" << std::endl;
      return -1;
    }
    if (!sql::bind(insert, 3, geoId))
    {
      std::cerr << "Error: Could not bind geoId value to prepared statement!" << std::endl;
      return -1;
    }
    if (!sql::bind(insert, 4, countryCode))
    {
      std::cerr << "Error: Could not bind country code value to prepared statement!" << std::endl;
      return -1;
    }
    if (!sql::bind(insert, 5, continent))
    {
      std::cerr << "Error: Could not bind continent value to prepared statement!" << std::endl;
      return -1;
    }
    const auto ret = sqlite3_step(insert.get());
    if ((ret != SQLITE_OK) && (ret != SQLITE_DONE))
    {
      std::cerr << "Error: Could not insert country data for " << name << " / "
                << geoId << " into database!" << std::endl;
      return -1;
    }

    const auto id = sqlite3_last_insert_rowid(db.get());
    if (id > 0)
      return id;
    else
    {
      std::cerr << "Error: Could not get id of last insert operation!" << std::endl;
      return -1;
    }
  }

  // Something unexpected happened here!
  std::cerr << "Error: Unexpected return value from sqlite3_step: " << rc << "!" << std::endl;
  return -1;
}

int64_t getCountryId(sql::database& db, const std::string& country)
{
  sql::statement stmt = sql::prepare(db, "SELECT countryId FROM country WHERE geoId=@country OR name=@country LIMIT 1;");
  if (!stmt)
    return -1;
  if (!sql::bind(stmt, 1, country))
    return -1;
  const auto rc = sqlite3_step(stmt.get());
  if (rc == SQLITE_ROW)
    return sqlite3_column_int64(stmt.get(), 0);

  // Something went wrong, maybe country was not found.
  return -1;
}

CovidNumbers getCountryData(sql::database& db, const int64_t countryId)
{
  auto stmt = sql::prepare(db, "SELECT SUM(cases), SUM(deaths) FROM covid19 WHERE countryId = @cid;");
  if (!stmt)
  {
    return {-1, -1 , {}};
  }
  if (!sql::bind(stmt, 1, countryId))
  {
    return {-1, -1 , {}};
  }
  int rc = sqlite3_step(stmt.get());
  if (rc != SQLITE_ROW)
    return {-1, -1 , {}};
  CovidNumbers result;
  result.totalCases = sqlite3_column_int64(stmt.get(), 0);
  result.totalDeaths = sqlite3_column_int64(stmt.get(), 1);

  stmt = sql::prepare(db, "SELECT date, cases, deaths FROM covid19 WHERE countryId = @cid ORDER BY date DESC LIMIT 7;");
  if (!stmt)
  {
    return {-1, -1 , {}};
  }
  if (!sql::bind(stmt, 1, countryId))
  {
    return {-1, -1 , {}};
  }
  while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW)
  {
    CovidNumbersElem elem;
    elem.cases = sqlite3_column_int64(stmt.get(), 1);
    elem.deaths = sqlite3_column_int64(stmt.get(), 2);
    elem.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
    result.days.emplace_back(elem);
  }
  if ((rc != SQLITE_OK) && (rc != SQLITE_DONE))
    return {-1, -1 , {}};

  return result;
}


Corona::Corona()
: dbLocation(std::optional<std::pair<std::string, std::chrono::steady_clock::time_point> >())
{
}

std::vector<std::string> Corona::commands() const
{
  return { "corona" };
}

Message Corona::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (command == "corona")
  {
    const auto now = std::chrono::steady_clock::now();
    if (!dbLocation.has_value() || ((now - dbLocation.value().second) > std::chrono::hours(6)))
    {
      const auto db = createDatabase();
      if (!db)
      {
        std::cerr << "Error: Database creation failed!" << std::endl;
        return Message("Could not get case numbers for COVID-19.");
      }
      std::string home;
      std::string dbFileName = "/tmp/corona.db";
      if (filesystem::getHome(home))
      {
        const auto delim = filesystem::pathDelimiter;
        dbFileName = home + delim + ".bvn" + delim + std::string("corona.db");
      }
      try
      {
        std::filesystem::rename(db.value(), dbFileName);
      }
      catch(const std::exception& ex)
      {
        std::cerr << "Error: Could not move file " << db.value() << " to "
                  << dbFileName << "!\nException message: " << ex.what() << std::endl;
        return Message("Could not get current case numbers for COVID-19.");
      }

      dbLocation = { dbFileName, now };
    }

    auto db = sql::open(dbLocation.value().first);
    if (!db)
    {
      std::clog << "Failed to open database " << dbLocation.value().first << std::endl;
      return Message("Could not open database containing COVID-19 case numbers!");
    }
    std::string country = std::string(message.substr(command.size()));
    trim(country);
    if (country.empty())
      country = "DE";
    const int64_t countryId = getCountryId(db, country);
    if (countryId == -1)
    {
      return Message("Could not find COVID-19 case numbers for " + country + "!");
    }

    const CovidNumbers data = getCountryData(db, countryId);
    if (data.totalCases == -1)
    {
      return Message("Could not get case numbers from database!");
    }

    Message result;
    for (const CovidNumbersElem& elem : data.days)
    {
      result.body.append("\n* ").append(elem.date).append(": ").append(std::to_string(elem.cases))
                 .append(" infection(s), ").append(std::to_string(elem.deaths)).append(" death(s)\n");
    }
    result.body.append("Total cases: " + std::to_string(data.totalCases))
                .append(", total deaths: " + std::to_string(data.totalDeaths));
    return result;
  }

  // unknown command
  return Message();
}

std::string Corona::helpOneLine(const std::string_view& command) const
{
  if (command == "corona")
  {
    return "shows current corona case numbers";
  }

  return std::string();
}

std::optional<std::string> Corona::createDatabase()
{
  Curly curl;
  curl.setURL("https://opendata.ecdc.europa.eu/covid19/casedistribution/csv/");
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Failed to get COVID-19 case numbers from EU's Open Data Portal!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  return buildDatabase(response);
}

std::optional<std::string> Corona::buildDatabase(const std::string& csv)
{
  std::clog << "Info: Building new database..." << std::endl;
  std::istringstream stream(csv);
  std::string line;

  if (!std::getline(stream, line))
  {
    std::cerr << "Error: Failed to read first line from CSV data!" << std::endl;
    return std::optional<std::string>();
  }
  if (line.at(line.length() - 1) == '\r')
  {
    line.erase(line.length() - 1);
  }

  if (line != "dateRep,day,month,year,cases,deaths,countriesAndTerritories,geoId,countryterritoryCode,popData2018,continentExp")
  {
    std::cerr << "Error: Header line of CSV data does not match the expected format!" << std::endl
              << "Header line is '" << line << "'." << std::endl;
    return std::optional<std::string>();
  }

  const std::string dbFileName = getTempFileName();

  sql::database db = sql::open(dbFileName);
  if (!db)
  {
    std::cerr << "Error: Could not create a new sqlite3 database!" << std::endl;
    return std::optional<std::string>();
  }
  if (!createDbStructure(db))
  {
    std::cerr << "Error: Database structure could not be created!" << std::endl;
    return std::optional<std::string>();
  }

  std::string lastGeoId;
  int64_t countryId = -1;

  unsigned long int lineCount = 0;
  while (std::getline(stream, line))
  {
    ++lineCount;
    if (lineCount % 1000 == 0)
    {
      std::clog << "Info: Processed " << lineCount << " lines." << std::endl;
    }
    if (line.empty())
      continue;

    // check for possible carriage return at end (happens on Windows systems)
    if (line.at(line.length() - 1) == '\r')
    {
      line.erase(line.length() - 1);
    }

    const auto parts = split(line, ',');
    if (parts.size() != 11)
    {
      std::cerr << "Error: A line of CSV data does not have eleven data elements, but "
                << parts.size() << " elements instead!" << std::endl
                << "The line is '" << line << "'. It will be skipped." << std::endl;
      continue;
      // return std::optional<std::string>();
    }
    const std::string currentGeoId = parts.at(7);
    if (currentGeoId.empty())
    {
      std::cerr << "Error: A line of CSV data does not have a geoId!" << std::endl
                << "The line is '" << line << "'." << std::endl;
      return std::optional<std::string>();
    }
    if (currentGeoId != lastGeoId)
    {
      // New country, insert it into database.
      const auto& name = parts.at(6);
      const auto& countryCode = parts.at(8);
      const auto& population = getInt64(parts.at(9));
      const auto& continent = parts.at(10);

      countryId = getCountryId(db, currentGeoId, name, population, countryCode, continent);
      if (countryId == -1)
      {
        std::cerr << "Error: Could not find id for geographic code '" << currentGeoId << "'!" << std::endl;
        return std::optional<std::string>();
      }
      lastGeoId = currentGeoId;
    }

    const auto& dayStr = parts.at(1);
    const auto& monthStr = parts.at(2);
    const auto& yearStr = parts.at(3);
    const int64_t cases = getInt64(parts.at(4));
    const int64_t deaths = getInt64(parts.at(5));
    if (cases == -1 || deaths == -1)
    {
      std::cerr << "Error: Got invalid case numbers in the following line:\n" << line << std::endl;
      return std::optional<std::string>();
    }
    const std::string date = yearStr + "-" + std::string(monthStr.size() == 1, '0') + monthStr + "-" + std::string(dayStr.size() == 1, '0') + dayStr;

    auto insertStmt = sql::prepare(db, "INSERT INTO covid19 (countryId, date, cases, deaths) VALUES (@cid, @date, @cases, @deaths);");
    if (!insertStmt)
    {
      std::cerr << "Error: Could not prepare insert statement for sqlite3 database!" << std::endl;
      return std::optional<std::string>();
    }
    if (!sql::bind(insertStmt, 1, countryId))
    {
      std::cerr << "Error: Could not bind countryId to prepared statement!" << std::endl;
      return std::optional<std::string>();
    }
    if (!sql::bind(insertStmt, 2, date) || !sql::bind(insertStmt, 3, cases) || !sql::bind(insertStmt, 4, deaths))
    {
      std::cerr << "Error: Could not bind values to insert statement!" << std::endl;
      return std::optional<std::string>();
    }

    const int rcInsert = sqlite3_step(insertStmt.get());
    if ((rcInsert != SQLITE_OK) && (rcInsert != SQLITE_DONE))
    {
      std::cerr << "Error: Could not insert case number for line '" << line
                << "' into database!" << std::endl;
      return std::optional<std::string>();
    }
  }

  return dbFileName;
}

} // namespace
