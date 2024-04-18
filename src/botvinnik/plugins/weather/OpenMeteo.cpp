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

#include "OpenMeteo.hpp"
#include <cmath>
#include <iostream>
#include <sstream>
#include "../../../../third-party/simdjson/simdjson.h"
#include "../../../net/Curly.hpp"

namespace bvn
{

std::string doubleToString(const double d)
{
  std::stringstream s_str;
  s_str << d;
  return s_str.str();
}

nonstd::expected<WeatherData, std::string> OpenMeteo::get_weather(const Location& location)
{
  if (std::isnan(location.latitude) || std::isnan(location.longitude))
  {
    return nonstd::make_unexpected("Location has no latitude and longitude!");
  }
  Curly curl;
  curl.setURL("https://api.open-meteo.com/v1/forecast?latitude=" + doubleToString(location.latitude)
      + "&longitude=" + doubleToString(location.longitude)
      + "&current=temperature_2m,relative_humidity_2m,apparent_temperature,"
      + "precipitation,weather_code,surface_pressure,wind_speed_10m,wind_direction_10m&timezone=auto");
  std::string response;
  if (!curl.perform(response))
  {
    std::cerr << "Error: Request to Open-Meteo failed!\n"
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return nonstd::make_unexpected("Failed to look up the requested location!");
  }

  return parse_response(response);
}

nonstd::expected<WeatherData, std::string> OpenMeteo::parse_response(const std::string& response)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from Open-Meteo!\n"
              << "Response is: " << response << std::endl;
    return nonstd::make_unexpected("The weather data request returned invalid JSON.");
  }

  if (doc.type() != simdjson::dom::element_type::OBJECT)
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: Root element is not an object!");
  }

  simdjson::dom::element current;
  error = doc.at_pointer("/current").get(current);
  if (error || (current.type() != simdjson::dom::element_type::OBJECT))
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: 'current' element is missing not an object!");
  }

  simdjson::dom::element element;
  error = current["temperature_2m"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: temperature_2m element is missing or not a floating-point number!");
  }
  WeatherData data;
  data.temperature_celsius = element.get<double>().value();

  error = current["apparent_temperature"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: apparent_temperature element is missing or not a floating-point number!");
  }
  data.apparent_temperature = element.get<double>().value();

  error = current["relative_humidity_2m"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: relative_humidity_2m element is missing or not an integer number!");
  }
  data.relative_humidity = element.get<int64_t>().value();

  error = current["weather_code"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: weather_code element is missing or not an integer number!");
  }
  data.weather_code = element.get<int64_t>().value();

  error = current["wind_speed_10m"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: wind_speed_10m element is missing or not a floating-point number!");
  }
  data.wind_speed = element.get<double>().value();

  error = current["wind_direction_10m"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: wind_direction_10m element is missing or not a floating-point number!");
  }
  data.wind_direction = element.get<double>().value();

  error = current["surface_pressure"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: surface_pressure element is missing or not a floating-point number!");
  }
  data.pressure = element.get<double>().value();

  error = current["precipitation"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: precipitation element is missing or not a floating-point number!");
  }
  data.precipitation = element.get<double>().value();

  return data;
}

} // namespace
