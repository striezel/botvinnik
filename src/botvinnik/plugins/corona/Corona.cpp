/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021  Dirk Stolle

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
#include <limits>
#include <random>
#include <regex>
#include <sstream>
#include <sqlite3.h>
#include "../../../net/Curly.hpp"
#include "../../../util/Directories.hpp"
#include "../../../util/sqlite3.hpp"
#include "../../../util/Strings.hpp"
#include "CovidNumbers.hpp"
#include "World.hpp"

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
      return std::numeric_limits<int64_t>::min();
    }
    return i;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not convert value '" << value << "' to int64_t!" << std::endl;
    return std::numeric_limits<int64_t>::min();
  }
}

double getDouble(const std::string& value)
{
  try
  {
    std::size_t pos;
    const double d = std::stod(value, &pos);
    if (pos != value.size())
    {
      std::cerr << "Error: '"<< value << "' is not a valid floating point value!";
      return std::numeric_limits<double>::quiet_NaN();
    }
    return d;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not convert value '" << value << "' to double!" << std::endl;
    return std::numeric_limits<double>::quiet_NaN();
  }
}

std::string roundTo2(const double d)
{
  std::ostringstream stream;
  stream << std::round(d * 100.0) / 100.0;
  return stream.str();
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
          deaths INTEGER,
          incidence14 REAL
        );
        )SQL";
  if (!sql::exec(db, statement))
  {
    std::cerr << "Error: Could not create tables in sqlite3 database!" << std::endl;
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
    if (!sql::bind(insert, 1, name) || !sql::bind(insert, 2, pop) || !sql::bind(insert, 3, geoId)
        || !sql::bind(insert, 4, countryCode) || !sql::bind(insert, 5, continent))
    {
      std::cerr << "Error: Could not bind values to prepared statement!" << std::endl;
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

Country getCountryId(sql::database& db, const std::string& country)
{
  sql::statement stmt = sql::prepare(db, "SELECT countryId, name, geoId FROM country WHERE geoId=@country OR name=@country LIMIT 1;");
  if (!stmt)
    return { -1, "", "" };
  if (!sql::bind(stmt, 1, country))
    return { -1, "", "" };
  const auto rc = sqlite3_step(stmt.get());
  if (rc == SQLITE_ROW)
  {
    Country result(sqlite3_column_int64(stmt.get(), 0));
    const char * name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
    if (name != nullptr)
      result.name = name;
    const char * geo = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
    if (geo != nullptr)
      result.geoId = geo;
    return result;
  }

  // Something went wrong, maybe country was not found.
  return { -1, "", "" };
}

CovidNumbers getCountryData(sql::database& db, const int64_t countryId)
{
  auto stmt = sql::prepare(db, "SELECT SUM(cases), SUM(deaths) FROM covid19 WHERE countryId = @cid;");
  if (!stmt)
  {
    return CovidNumbers();
  }
  if (!sql::bind(stmt, 1, countryId))
  {
    return CovidNumbers();
  }
  int rc = sqlite3_step(stmt.get());
  if (rc != SQLITE_ROW)
    return CovidNumbers();
  CovidNumbers result;
  result.totalCases = sqlite3_column_int64(stmt.get(), 0);
  result.totalDeaths = sqlite3_column_int64(stmt.get(), 1);

  stmt = sql::prepare(db, "SELECT date, cases, deaths, ifnull(incidence14, -20.0) FROM covid19 WHERE countryId = @cid ORDER BY date DESC LIMIT 10;");
  if (!stmt)
  {
    return CovidNumbers();
  }
  if (!sql::bind(stmt, 1, countryId))
  {
    return CovidNumbers();
  }
  while ((rc = sqlite3_step(stmt.get())) == SQLITE_ROW)
  {
    CovidNumbersElem elem;
    elem.cases = sqlite3_column_int64(stmt.get(), 1);
    elem.deaths = sqlite3_column_int64(stmt.get(), 2);
    elem.incidence14 = sqlite3_column_double(stmt.get(), 3);
    if (elem.incidence14 < 0.0)
    {
      elem.incidence14 = std::numeric_limits<double>::quiet_NaN();
    }
    elem.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
    result.days.emplace_back(elem);
  }
  if ((rc != SQLITE_OK) && (rc != SQLITE_DONE))
    return CovidNumbers();

  return result;
}

CovidNumbers getWorldData(sql::database& db)
{
  auto stmt = sql::prepare(db, "SELECT SUM(cases), SUM(deaths) FROM covid19;");
  if (!stmt)
  {
    return CovidNumbers();
  }
  int rc = sqlite3_step(stmt.get());
  if (rc != SQLITE_ROW)
    return CovidNumbers();
  CovidNumbers result;
  result.totalCases = sqlite3_column_int64(stmt.get(), 0);
  result.totalDeaths = sqlite3_column_int64(stmt.get(), 1);

  stmt = sql::prepare(db, "SELECT date, SUM(cases), SUM(deaths) FROM covid19 GROUP BY date ORDER BY date DESC LIMIT 10;");
  if (!stmt)
  {
    return CovidNumbers();
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
    return CovidNumbers();

  return result;
}


Corona::Corona(Matrix& matrix)
: dbLocation(std::optional<std::pair<std::string, std::chrono::steady_clock::time_point> >()),
  theMatrix(matrix)
{
}

std::vector<std::string> Corona::commands() const
{
  return { "corona" };
}

Message Corona::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts)
{
  if (command == "corona")
  {
    std::string name = std::string(message.substr(command.size()));
    trim(name);
    Message result;
    if (name.empty())
    {
      name = "Germany"; // Germany is the default country, if none is given.
      result.body = "No country was specified - assuming " + name + ".\n";
      result.formatted_body = "<em>No country was specified - assuming " + name + ".</em><br />\n";
    }

    const bool worldwide = toLowerString(name) == "world" || toLowerString(name) == "the world"
                           || toLowerString(name) == "all";
    const auto country_opt = !worldwide ? World::find(name) : std::optional<Country>(Country(0, "the world", "all"));
    if (!country_opt.has_value() || country_opt.value().countryId == -1)
    {
      return Message("Could not find COVID-19 case numbers for '" + name
                   + "'. Use a two letter country code (ISO 3166) or the English name of the country."
                   + " If you want worldwide case numbers, use 'world' or 'all' instead.",
                   "Could not find COVID-19 case numbers for '" + name
                   + "'. Use a two letter country code (see <a href=\"https://en.wikipedia.org/wiki/ISO_3166\">ISO 3166</a>) or the English name of the country."
                   + " If you want worldwide case numbers, use 'world' or 'all' instead.");
    }
    const auto& country = country_opt.value();

    CovidNumbers data = World::getCountryData(country);
    if (data.totalCases == -1)
    {
      return Message("Could not get case numbers from API!");
    }
    // Post-processing of retrieved data: Calculate 14-day incidence, ...
    data.days = calculate_incidence(data.days, country.population);
    // ... and start with newest instead of oldest data, ...
    std::reverse(data.days.begin(), data.days.end());
    // ... and cut it down to no more than ten elements.
    if (data.days.size() > 10)
    {
      data.days.resize(10);
    }

    result.body.append("Corona cases in " + country.name + " (" + country.geoId + "):\n");
    result.formatted_body.append("Corona cases in " + country.name + " (" + country.geoId + "):<br />\n<ul>");
    bool hasIncidence = false;
    for (const CovidNumbersElem& elem : data.days)
    {
      result.body.append("\n* ").append(elem.date).append(": ").append(std::to_string(elem.cases))
                 .append(" infection(s), ").append(std::to_string(elem.deaths)).append(" death(s)");
      result.formatted_body.append("\n  <li>").append(elem.date).append(": ").append(std::to_string(elem.cases))
                 .append(" infection(s), ").append(std::to_string(elem.deaths)).append(" death(s)");
      if (!std::isnan(elem.incidence14))
      {
        hasIncidence = true;
        result.body.append(", 14-day incidence: ").append(roundTo2(elem.incidence14));
        result.formatted_body.append(", 14-day incidence<sup>1</sup>: ").append(roundTo2(elem.incidence14));
      }
      result.formatted_body.append("</li>");
    }
    const auto percentage = data.percentage();
    result.body.append("\nTotal cases: " + std::to_string(data.totalCases))
                .append(", total deaths: " + std::to_string(data.totalDeaths));
    if (!percentage.empty())
      result.body.append(" (" + percentage + ")");
    if (hasIncidence)
    {
      result.body.append("\n\nThe 14-day incidence is the number of infections during the last 14 days per 100000 inhabitants.")
                 .append(" Note that some authorities like e. g. Germany's Robert Koch Institute use a 7-day incidence value instead, which is different.");
    }
    result.body.append("\n\nData source: https://github.com/CSSEGISandData/COVID-19, ")
                .append("provided by the Center for Systems Science and Engineering (CSSE) at Johns Hopkins University");
    result.formatted_body.append("\n</ul><br>\n<b>Total cases: " + std::to_string(data.totalCases))
                .append(", total deaths: " + std::to_string(data.totalDeaths));
    if (!percentage.empty())
      result.formatted_body.append(" (" + percentage + ")");
    result.formatted_body.append("</b>");
    if (hasIncidence)
    {
      result.formatted_body.append("<br />\n<br />\n<sup>1</sup>=The 14-day incidence is the number of infections during the last 14 days per 100000 inhabitants.")
                 .append(" Note that some authorities like e. g. Germany's Robert Koch Institute use a 7-day incidence value instead, which is different.");
    }
    result.formatted_body.append("<br />\n<br />\nData source: https://github.com/CSSEGISandData/COVID-19, ")
                .append("provided by the Center for Systems Science and Engineering (CSSE) at Johns Hopkins University");
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
  std::clog << "Info: Building new database from CSV..." << std::endl;
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
  if (line.length() > 3)
  {
    // Remove UTF-8 byte order mark. It just confuses the following checks.
    if (line.substr(0, 3) == "\xEF\xBB\xBF")
      line.erase(0, 3);
  }

  const std::regex header("dateRep,day,month,year,cases,deaths,countriesAndTerritories,geoId,countryterritoryCode,popData2[0-9]{3},continentExp,Cumulative_number_for_14_days_of_COVID-19_cases_per_100000");
  std::smatch match;
  if (!std::regex_match(line, match, header))
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
  std::string batch;
  unsigned int batchCount = 0;
  while (std::getline(stream, line))
  {
    ++lineCount;
    if (line.empty())
      continue;

    // check for possible carriage return at end (happens on Windows systems)
    if (line.at(line.length() - 1) == '\r')
    {
      line.erase(line.length() - 1);
    }

    auto parts = split(line, ',');
    if (parts.size() != 12)
    {
      if (parts.size() == 13 && parts[6] == "\"Bonaire" && parts[7] == " Saint Eustatius and Saba\"")
      {
        parts[6] = "Bonaire, Saint Eustatius and Saba";
        parts.erase(parts.begin() + 7);
      }
      else
      {
        std::cerr << "Error: A line of CSV data does not have twelve data elements, but "
                  << parts.size() << " elements instead!" << std::endl
                  << "The line is '" << line << "'. It will be skipped." << std::endl;
        continue;
      }
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

      countryId = getCountryId(db, currentGeoId, name,
                               population != std::numeric_limits<int64_t>::min() ? population : -1,
                               countryCode, continent);
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
    const int64_t cases = !parts.at(4).empty() ? getInt64(parts.at(4)) : 0;
    const int64_t deaths = !parts.at(5).empty() ? getInt64(parts.at(5)) : 0;
    const double incidence14 = !parts.at(11).empty() ? getDouble(parts.at(11)) : std::numeric_limits<double>::quiet_NaN();
    if (cases == std::numeric_limits<int64_t>::min() || deaths == std::numeric_limits<int64_t>::min())
    {
      std::cerr << "Error: Got invalid case numbers in the following line:\n" << line << std::endl;
      return std::optional<std::string>();
    }
    const std::string date = yearStr + "-" + std::string(monthStr.size() == 1, '0') + monthStr + "-" + std::string(dayStr.size() == 1, '0') + dayStr;

    if (batch.empty())
    {
      batch = "INSERT INTO covid19 (countryId, date, cases, deaths, incidence14) VALUES ";
      batchCount = 0;
    }

    batch.append("(")
         .append(std::to_string(countryId)).append(", ")
         .append(sql::quote(date)).append(", ")
         .append(std::to_string(cases)).append(", ")
         .append(std::to_string(deaths)).append(", ")
         .append(std::isnan(incidence14) ? "NULL" : std::to_string(incidence14))
         .append("),");
    ++batchCount;

    // Perform one insert for every 250 data sets.
    if (batchCount >= 250 && !batch.empty())
    {
      batch.at(batch.size() - 1) = ';';
      if (!sql::exec(db, batch))
      {
        std::cerr << "Error: Could not batch-insert case numbers into database!" << std::endl;
        return std::optional<std::string>();
      }

      batch.clear();
      batchCount = 0;
    } // if batch
  }

  if (batchCount > 0 && !batch.empty())
  {
    batch.at(batch.size() - 1) = ';';
    if (!sql::exec(db, batch))
    {
      std::cerr << "Error: Could not batch-insert case numbers into database!" << std::endl;
      return std::optional<std::string>();
    }
    batch.clear();
    batchCount = 0;
  }

  // Replace underscores in names - those should be spaces.
  if (!sql::exec(db, "UPDATE country SET name = REPLACE(name, '_', ' ');"))
  {
    // Failure is not critical, only unpleasant.
    std::cerr << "Warning: Failed to replace underscores in country names with spaces!" << std::endl;
  }

  return dbFileName;
}

} // namespace
