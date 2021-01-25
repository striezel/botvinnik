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
#include <algorithm>
#include <cmath> // for std::isnan()
#include <iostream>
#include <optional>
#include <sstream>
#include "../../../util/Strings.hpp"
#include "CovidNumbers.hpp"
#include "World.hpp"

namespace bvn
{

std::string roundTo2(const double d)
{
  std::ostringstream stream;
  stream << std::round(d * 100.0) / 100.0;
  return stream.str();
}


Corona::Corona()
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

} // namespace
