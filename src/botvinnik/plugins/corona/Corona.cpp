/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022, 2023, 2024, 2025  Dirk Stolle

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
#include <algorithm>
#include <cmath> // for std::isnan()
#include <filesystem>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include "../../../net/Curly.hpp"
#include "../../../util/chrono.hpp"
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
    // Values for Our World in Data are actually floating-point numbers instead
    // of integers, so we have to convert to double first.
    const double d = std::stod(value, &pos);
    if (pos != value.size())
    {
      std::cerr << "Error: '"<< value << "' is not a valid floating-point number!";
      return std::numeric_limits<int64_t>::min();
    }
    if ((d > std::numeric_limits<int64_t>::max()) || (d < std::numeric_limits<int64_t>::min()))
    {
      std::cerr << "Error: '"<< value << "' is out of range for 64 bit integer!";
      return std::numeric_limits<int64_t>::min();
    }
    return static_cast<int64_t>(d);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not convert value '" << value << "' to int64_t!" << std::endl;
    return std::numeric_limits<int64_t>::min();
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

  return (dirPath / basename).string();
}

bool createDbStructure(sql::database& db)
{
  const std::string statement = R"SQL(
        CREATE TABLE country (
          countryId INTEGER PRIMARY KEY NOT NULL,
          name TEXT NOT NULL,
          population INTEGER,
          geoId TEXT NOT NULL
        );
        CREATE TABLE covid19 (
          countryId INTEGER NOT NULL,
          date TEXT,
          cases INTEGER,
          deaths INTEGER
        );
        )SQL";
  if (!sql::exec(db, statement))
  {
    std::cerr << "Error: Could not create tables in sqlite3 database!" << std::endl;
    return false;
  }

  return true;
}


int64_t getCountryId(sql::database& db, const std::string& geoId,
                     const std::string& name, const int64_t pop)
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
  {
    return sqlite3_column_int64(stmt.get(), 0);
  }
  if (rc == SQLITE_DONE)
  {
    // Not found - insert it.
    auto insert = sql::prepare(db, "INSERT INTO country (name, population, geoId) VALUES (@countryname, @pop, @geo);");
    if (!insert)
    {
      std::cerr << "Error: Could not prepare insert statement for geoId!" << std::endl;
      return -1;
    }
    if (!sql::bind(insert, 1, name) || !sql::bind(insert, 2, pop) || !sql::bind(insert, 3, geoId))
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
    {
      return id;
    }
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

std::optional<Country> getCountry(sql::database& db, const std::string& country)
{
  sql::statement stmt = sql::prepare(db, "SELECT countryId, name, geoId, population FROM country WHERE lower(geoId)=lower(@country) OR lower(name)=lower(@country) LIMIT 1;");
  if (!stmt)
  {
    return std::nullopt;
  }
  if (!sql::bind(stmt, 1, country))
  {
    return std::nullopt;
  }
  const auto rc = sqlite3_step(stmt.get());
  if (rc == SQLITE_ROW)
  {
    Country result(sqlite3_column_int64(stmt.get(), 0));
    const char * name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
    if (name != nullptr)
    {
      result.name = name;
    }
    const char * geo = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
    if (geo != nullptr)
    {
      result.geoId = geo;
    }
    result.population = sqlite3_column_int64(stmt.get(), 3);
    return result;
  }

  // Something went wrong, maybe country was not found.
  return std::nullopt;
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
  {
    return CovidNumbers();
  }
  CovidNumbers result;
  result.totalCases = sqlite3_column_int64(stmt.get(), 0);
  result.totalDeaths = sqlite3_column_int64(stmt.get(), 1);

  stmt = sql::prepare(db, "SELECT date, cases, deaths FROM covid19 WHERE countryId = @cid ORDER BY date DESC LIMIT 25;");
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
    elem.incidence7 = std::numeric_limits<double>::quiet_NaN();
    elem.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
    result.days.emplace_back(elem);
  }
  if ((rc != SQLITE_OK) && (rc != SQLITE_DONE))
  {
    return CovidNumbers();
  }

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
  {
    return CovidNumbers();
  }
  CovidNumbers result;
  result.totalCases = sqlite3_column_int64(stmt.get(), 0);
  result.totalDeaths = sqlite3_column_int64(stmt.get(), 1);

  stmt = sql::prepare(db, "SELECT date, SUM(cases), SUM(deaths) FROM covid19 GROUP BY date ORDER BY date DESC LIMIT 25;");
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
    elem.incidence7 = std::numeric_limits<double>::quiet_NaN();
    result.days.emplace_back(elem);
  }
  if ((rc != SQLITE_OK) && (rc != SQLITE_DONE))
  {
    return CovidNumbers();
  }

  return result;
}

Corona::Corona(Matrix& matrix)
: dbLocation(std::nullopt),
  theMatrix(matrix)
{
}

std::vector<std::string> Corona::commands() const
{
  return { "corona" };
}

std::optional<std::string> Corona::createDatabase()
{
  Curly curl;
  curl.setURL("https://catalog.ourworldindata.org/garden/covid/latest/compact/compact.csv");
  curl.followRedirects(true);
  curl.setMaximumRedirects(5);
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Failed to get COVID-19 case numbers from Our World in Data!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  return buildDatabase(response);
}

std::optional<std::string> Corona::buildDatabase(const std::string& csv)
{
  std::clog << nowToString() << " Info: Building new database from CSV..." << std::endl;
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
    {
      line.erase(0, 3);
    }
  }

  const std::string header("country,date,total_cases,new_cases,new_cases_smoothed,total_cases_per_million,new_cases_per_million,new_cases_smoothed_per_million,total_deaths,new_deaths,");
  if ((line.find(header) != 0) || (line.find(",code,") == std::string::npos))
  {
    std::cerr << "Error: Header line of CSV data does not match the expected format!" << std::endl
              << "Header line is '" << line << "'." << std::endl;
    return std::nullopt;
  }
  const std::string dbFileName = getTempFileName();

  sql::database db = sql::open(dbFileName);
  if (!db)
  {
    std::cerr << "Error: Could not create a new sqlite3 database!" << std::endl;
    return std::nullopt;
  }
  if (!createDbStructure(db))
  {
    std::cerr << "Error: Database structure could not be created!" << std::endl;
    return std::nullopt;
  }

  /// zero-based index of the column that contains the country's name
  constexpr std::size_t idx_name = 0;

  /// zero-based index of the column that contains the date
  constexpr std::size_t idx_date = 1;

  /// zero-based index of the column containing the number of new cases
  constexpr std::size_t idx_cases = 3;

  /// zero-based index of the column containing the number of new deaths
  const std::size_t idx_deaths = 9;

  /// zero-based index of the column containing the ISO-3166 ALPHA-3 country code
  constexpr std::size_t idx_iso3 = 49;

  std::string lastGeoId;
  int64_t countryId = -1;

  std::string batch;
  unsigned int batchCount = 0;
  batch.reserve(160000);
  while (std::getline(stream, line))
  {
    if (line.empty())
    {
      continue;
    }

    // check for possible carriage return at end (happens on Windows systems)
    if (line.at(line.length() - 1) == '\r')
    {
      line.erase(line.length() - 1);
    }

    auto parts = split(line, ',');
    if (parts[0].find("World excl.") != std::string::npos)
    {
      continue;
    }
    if (parts.size() != 61)
    {
      std::cerr << "Error: A line of CSV data does not have 61 data elements, but "
                << parts.size() << " elements instead!\n"
                << "The line is '" << line << "'. It will be skipped.\n";
        continue;
    }
    // Workaround so that data for Kosovo is included.
    if (parts[idx_iso3] == "OWID_KOS")
    {
      parts[idx_iso3] = "KOS";
    }
    const std::string currentGeoId = parts.at(idx_iso3);
    if (currentGeoId.empty() || (currentGeoId.find("OWID_") != std::string::npos))
    {
      // Some of the OWID categories (e. g. "High-income countries") have no
      // proper ISO-3166 ALPHA-3 id. We can just skip those lines.
      continue;
    }
    if (currentGeoId != lastGeoId)
    {
      // New country, insert it into database.
      std::string name = parts[idx_name];
      const auto opt_country = World::find(name);
      const auto& population = opt_country.has_value() ? opt_country.value().population : -1;

      countryId = getCountryId(db, currentGeoId, name, population);
      if (countryId == -1)
      {
        std::cerr << "Error: Could not find id for geographic code '" << currentGeoId << "'!" << std::endl;
        return std::nullopt;
      }
      lastGeoId = currentGeoId;
    }

    const auto& date = parts[idx_date];
    const int64_t cases = !parts[idx_cases].empty() ? getInt64(parts[idx_cases]) : 0;
    const int64_t deaths = !parts[idx_deaths].empty() ? getInt64(parts[idx_deaths]) : 0;
    if (cases == std::numeric_limits<int64_t>::min() || deaths == std::numeric_limits<int64_t>::min())
    {
      std::cerr << "Error: Got invalid case numbers in the following line:\n" << line << std::endl;
      return std::nullopt;
    }

    if (batch.empty())
    {
      batch = "INSERT INTO covid19 (countryId, date, cases, deaths) VALUES ";
      batchCount = 0;
    }

    batch.append("(")
         .append(std::to_string(countryId)).append(", ")
         .append(sql::quote(date)).append(", ")
         .append(std::to_string(cases)).append(", ")
         .append(std::to_string(deaths))
         .append("),");
    ++batchCount;

    // Perform one insert for every 5000 data sets.
    if (batchCount >= 5000 && !batch.empty())
    {
      batch[batch.size() - 1] = ';';
      if (!sql::exec(db, batch))
      {
        std::cerr << "Error: Could not batch-insert case numbers into database!" << std::endl;
        return std::nullopt;
      }

      batch.clear();
      batchCount = 0;
    } // if batch
  }

  if (batchCount > 0 && !batch.empty())
  {
    batch[batch.size() - 1] = ';';
    if (!sql::exec(db, batch))
    {
      std::cerr << "Error: Could not batch-insert case numbers into database!" << std::endl;
      return std::nullopt;
    }
    batch.clear();
    batchCount = 0;
  }

  return dbFileName;
}

std::optional<Message> Corona::updateOldDatabase(const std::string_view& roomId)
{
  theMatrix.sendMessage(std::string(roomId),
                        Message(std::string("Getting current COVID-19 data. This may take a while ...\n")
                            + "(It may be ten seconds, it may be two minutes, depending on network and I/O speed.)"));
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
    const std::string directory = home + delim + ".bvn";
    dbFileName = directory + delim + std::string("corona.db");
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
  // Move database from temporary location to bot's directory.
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

  dbLocation = { dbFileName, std::chrono::steady_clock::now() };
  return std::nullopt;
}

Message Corona::handleCommand(const std::string_view& command, const std::string_view& message,
                              [[maybe_unused]] const std::string_view& userId,
                              const std::string_view& roomId,
                              [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if (command == "corona")
  {
    // Check whether database needs to be created or needs an update.
    const auto now = std::chrono::steady_clock::now();
    if (!dbLocation.has_value() || ((now - dbLocation.value().second) > std::chrono::hours(6)))
    {
      const auto opt_msg = updateOldDatabase(roomId);
      if (opt_msg.has_value())
      {
        return opt_msg.value();
      }
    }

    auto db = sql::open(dbLocation.value().first);
    if (!db)
    {
      std::clog << "Failed to open database " << dbLocation.value().first << std::endl;
      return Message("Could not open database containing COVID-19 case numbers!");
    }

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
    const auto country_opt = !worldwide ? getCountry(db, name) : std::optional<Country>(Country(0, "the world", "all"));
    if (!country_opt.has_value())
    {
      return Message("Could not find COVID-19 case numbers for '" + name
                   + "'. Use a three letter country code (ISO 3166) or the English name of the country."
                   + " If you want worldwide case numbers, use 'world' or 'all' instead.",
                   "Could not find COVID-19 case numbers for '" + name
                   + "'. Use a three letter country code (see <a href=\"https://en.wikipedia.org/wiki/ISO_3166\">ISO 3166</a>) or the English name of the country."
                   + " If you want worldwide case numbers, use 'world' or 'all' instead.");
    }
    const auto& country = country_opt.value();

    CovidNumbers data = !worldwide ? getCountryData(db, country.countryId) : getWorldData(db);
    if (data.totalCases == -1)
    {
      return Message("Could not get case numbers from database!");
    }
    // Post-processing of retrieved data: Calculate 7-day incidence.
    if (country.population > 0)
    {
      // Reverse data to start with oldest date instead of newest.
      std::reverse(data.days.begin(), data.days.end());
      // Calculate the incidence values.
      data.days = calculate_incidence(data.days, country.population);
      // Reverse again to start with newest instead of oldest data.
      std::reverse(data.days.begin(), data.days.end());
    }
    // Cut it down to no more than ten elements.
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
      if (!std::isnan(elem.incidence7))
      {
        hasIncidence = true;
        result.body.append(", 7-day incidence: ").append(roundTo2(elem.incidence7));
        result.formatted_body.append(", 7-day incidence<sup>1</sup>: ").append(roundTo2(elem.incidence7));
      }
      result.formatted_body.append("</li>");
    }
    const auto percentage = data.percentage();
    result.body.append("\nTotal cases: " + std::to_string(data.totalCases))
                .append(", total deaths: " + std::to_string(data.totalDeaths));
    if (!percentage.empty())
    {
      result.body.append(" (" + percentage + ")");
    }
    if (hasIncidence)
    {
      result.body.append("\n\nThe 7-day incidence is the number of infections during the last seven days per 100000 inhabitants.");
    }
    result.body.append("\n\nData source: https://ourworldindata.org/coronavirus, ")
                .append("provided by Our World in Data");
    result.formatted_body.append("\n</ul><br />\n<b>Total cases: " + std::to_string(data.totalCases))
                .append(", total deaths: " + std::to_string(data.totalDeaths));
    if (!percentage.empty())
    {
      result.formatted_body.append(" (" + percentage + ")");
    }
    result.formatted_body.append("</b>");
    if (hasIncidence)
    {
      result.formatted_body.append("<br />\n<br />\n<sup>1</sup>=The 7-day incidence is the number of infections during the last seven days per 100000 inhabitants.");
    }
    result.formatted_body.append("<br />\n<br />\nData source: https://ourworldindata.org/coronavirus, ")
                .append("provided by Our World in Data");
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

Message Corona::helpExtended(const std::string_view& command, const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "corona")
  {
    return Message("gets current COVID-19 case numbers. The country for which"s
      + " to get the data can be specified either by its English name (e. g. `"s
      .append(prefix) + "corona Germany`) or by its three letter code from ISO "s
      + "3166 (e. g. `"s .append(prefix) + "corona DEU` for Germany's data). "
      + "If you want to show summarized worldwide case numbers, just type `"s
      .append(prefix) + "corona world` or `"s .append(prefix) + "corona all`"s
      + " to show the total amount of COVID-19 cases in the world.",
      "gets current COVID-19 case numbers. The country for which"s
      + " to get the data can be specified either by its English name (e. g. <code>"s
      .append(prefix) + "corona Germany</code>) or by its three letter code from ISO "s
      + "3166 (e. g. <code>"s .append(prefix) + "corona DEU</code> for Germany's"
      + " data). If you want to show summarized worldwide case numbers, just "
      + "type <code>"s .append(prefix) + "corona world</code> or <code>"s
      .append(prefix) + "corona all</code> to show the total amount of COVID-19"
      + " cases in the world.");
  }

  return Message();
}

} // namespace
