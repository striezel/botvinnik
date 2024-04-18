/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2024  Dirk Stolle

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

#include "Weather.hpp"
#include <iostream>
#include "../../../util/Strings.hpp"
#include "LocationLookup.hpp"
#include "OpenMeteo.hpp"

namespace bvn
{

std::vector<std::string> Weather::commands() const
{
  return { "weather" };
}

Message Weather::handleCommand(const std::string_view& command, const std::string_view& message,
                             [[maybe_unused]] const std::string_view& userId,
                             [[maybe_unused]] const std::string_view& roomId,
                             [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if (command != "weather")
  {
    // unknown command
    return Message();
  }

  std::string query(message.substr(command.size()));
  trim(query);
  if (query.empty())
  {
    return Message(std::string("Please enter a location to get the weather for after the '").append(command).append("' command."));
  }
  const auto location = LocationLookup::find_location(query);
  if (!location.has_value())
  {
    std::cerr << "Location lookup for weather plugin failed!\n"
              << location.error() << "\n";
    return Message("Could not find a geographical location named '" + query
                 + "'. The weather command expects the name of a city, e. g. "
                 + "Berlin for the city of Berlin in Germany, or something similar.");
  }
  const auto weather = OpenMeteo::get_weather(location.value());
  if (!weather.has_value())
  {
    std::cerr << "Failed to get weather data!\n" << weather.error() << "\n";
    return Message("Could get weather data for '" + location.value().name + "'.");
  }

  const WeatherData& data = weather.value();
  return Message("Weather for " + location.value().display_name + "\n"
      + doubleToString(data.temperature_celsius) + " °C, feels like "
      + doubleToString(data.apparent_temperature) + " °C\n"
      + "Relative humidity: " + std::to_string(data.relative_humidity) + " %\n"
      + "Wind speed: " + doubleToString(data.wind_speed) + " km/h\n"
      + "Pressure: " + doubleToString(data.pressure) + " hPa\n"
      + "Precipitation: " + doubleToString(data.precipitation) + " mm",
      "<strong>Weather for " + location.value().display_name + "</strong><br />\n"
      + doubleToString(data.temperature_celsius) + " °C, feels like "
      + doubleToString(data.apparent_temperature) + " °C<br />\n"
      + "Relative humidity: " + std::to_string(data.relative_humidity) + " %<br />\n"
      + "Wind speed: " + doubleToString(data.wind_speed) + " km/h<br />\n"
      + "Pressure: " + doubleToString(data.pressure) + " hPa<br />\n"
      + "Precipitation: " + doubleToString(data.precipitation) + " mm"           );
}

std::string Weather::helpOneLine(const std::string_view& command) const
{
  if (command == "weather")
  {
    return "displays weather data for a given location";
  }

  return std::string();
}

Message Weather::helpExtended(const std::string_view& command, const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "weather")
  {
    return Message("displays weather data for a given location. For example, `"s
        .append(prefix) + "weather Berlin` will show the current weather in Berlin, Germany.",
        "displays weather data for a given location. For example, <code>"s
        .append(prefix) + "weather Berlin</code> will show the current weather in Berlin, Germany.");
  }

  return Message();
}

} // namespace
