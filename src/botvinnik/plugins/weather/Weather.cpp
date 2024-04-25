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
#include "FreeFunctions.hpp"
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

  // add current weather data
  const CurrentData& data = weather.value().current;
  Message msg = Message("Weather for " + location.value().display_name + "\n"
      + weather::wmo_code_to_icon(data.weather_code) + " "
      + weather::wmo_code_to_text(data.weather_code) + ", "
      + doubleToString(data.temperature_celsius) + " °C, feels like "
      + doubleToString(data.apparent_temperature) + " °C\n"
      + "Relative humidity: " + std::to_string(data.relative_humidity) + " %\n"
      + "Wind: " + doubleToString(data.wind_speed) + " km/h "
                 + weather::wind_direction_to_arrow(data.wind_direction)
                 + " " + weather::wind_direction_to_text(data.wind_direction) + "\n"
      + "Pressure: " + doubleToString(data.pressure) + " hPa\n"
      + "Precipitation: " + doubleToString(data.precipitation) + " mm\n\n"
      + "Forecast:",
      "<strong>Weather for " + location.value().display_name + "</strong><br />\n"
      + weather::wmo_code_to_icon(data.weather_code) + " "
      + weather::wmo_code_to_text(data.weather_code) + ", "
      + doubleToString(data.temperature_celsius) + " °C, feels like "
      + doubleToString(data.apparent_temperature) + " °C<br />\n"
      + "Relative humidity: " + std::to_string(data.relative_humidity) + " %<br />\n"
      + "Wind: " + doubleToString(data.wind_speed) + " km/h "
                 + weather::wind_direction_to_arrow(data.wind_direction)
                 + " " + weather::wind_direction_to_text(data.wind_direction) + "<br />\n"
      + "Pressure: " + doubleToString(data.pressure) + " hPa<br />\n"
      + "Precipitation: " + doubleToString(data.precipitation) + " mm<br />\n<br />\n"
      + "<strong>Forecast:</strong>");
  // add forecast weather data
  const std::string spacer{"&emsp;&emsp;"};
  for (const auto& elem: weather.value().forecast)
  {
    msg.body.append("\n" + elem.date + ":\t" + doubleToString(elem.temperature_min)
                    + " °C / " + doubleToString(elem.temperature_max) + " °C\t"
                    + weather::wmo_code_to_icon(elem.weather_code) + " "
                    + weather::wmo_code_to_text(elem.weather_code));
    msg.formatted_body.append("<br />\n" + elem.date + ":" + spacer + doubleToString(elem.temperature_min)
                    + " °C / " + doubleToString(elem.temperature_max) + " °C"
                    + spacer + weather::wmo_code_to_icon(elem.weather_code)
                     + " " + weather::wmo_code_to_text(elem.weather_code));
  }
  // add Open-Meteo attribution
  msg.body.append("\n\nWeather data by Open-Meteo.com <https://open-meteo.com/>");
  msg.formatted_body.append("<br />\n<br />\n<a href=\"https://open-meteo.com/\">Weather data by Open-Meteo.com</a>");
  return msg;
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
        .append(prefix) + "weather Berlin` will show the current weather in "
        + "Berlin, Germany, as well as a forecast for the next few days.",
        "displays weather data for a given location. For example, <code>"s
        .append(prefix) + "weather Berlin</code> will show the current weather"
        + " in Berlin, Germany, as well as a forecast for the next few days.");
  }

  return Message();
}

} // namespace
