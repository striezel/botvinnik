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
#include "../../../net/Curly.hpp"
#include "../../../util/Strings.hpp"

namespace bvn
{

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
      + "precipitation,weather_code,surface_pressure,wind_speed_10m,wind_direction_10m"
      + "&daily=weather_code,temperature_2m_max,temperature_2m_min"
      + "&timezone=auto");
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
  const auto error = parser.parse(response).get(doc);
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

  auto current = parse_current_data(doc);
  if (!current.has_value())
  {
    return nonstd::make_unexpected(current.error());
  }

  auto forecast = parse_forecast_data(doc);
  if (!forecast.has_value())
  {
    return nonstd::make_unexpected(forecast.error());
  }

  WeatherData w;
  w.current = current.value();
  w.forecast = forecast.value();
  return w;
}

nonstd::expected<CurrentData, std::string> OpenMeteo::parse_current_data(const simdjson::dom::element& doc)
{
  simdjson::dom::element current;
  auto error = doc.at_pointer("/current").get(current);
  if (error || (current.type() != simdjson::dom::element_type::OBJECT))
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: 'current' element is missing or not an object!");
  }

  simdjson::dom::element element;
  error = current["temperature_2m"].get(element);
  if (error || !element.is_double())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: temperature_2m element is missing or not a floating-point number!");
  }
  CurrentData data;
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

nonstd::expected<std::vector<ForecastData>, std::string> OpenMeteo::parse_forecast_data(const simdjson::dom::element& doc)
{
  simdjson::dom::element daily;
  auto error = doc.at_pointer("/daily").get(daily);
  if (error || (daily.type() != simdjson::dom::element_type::OBJECT))
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: 'daily' element is missing or not an object!");
  }

  simdjson::dom::element element;
  error = daily["time"].get(element);
  if (error || !element.is_array())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.time element is missing or not an array!");
  }
  simdjson::dom::array array = element.get_array().value();
  const auto count = array.size();
  if (count == 0)
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.time element is an empty array!");
  }

  std::vector<ForecastData> data;
  for (auto time: array)
  {
    if (!time.is_string())
    {
      return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: An element of daily.time is not a string!");
    }
    ForecastData fc;
    fc.date = time.get<std::string_view>().value();
    data.emplace_back(fc);
  }

  error = daily["weather_code"].get(element);
  if (error || !element.is_array())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.weather_code element is missing or not an array!");
  }
  array = element.get_array().value();
  if (array.size() != count)
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.weather_code array has an unexpected number of elements!");
  }
  {
    std::size_t idx = 0;
    for (const auto& code: array)
    {
      if (!code.is<int64_t>())
      {
        return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: An element of daily.weather_code is not an integer!");
      }
      data[idx].weather_code = code.get<int64_t>().value();
      ++idx;
    }
  }

  error = daily["temperature_2m_max"].get(element);
  if (error || !element.is_array())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.temperature_2m_max element is missing or not an array!");
  }
  array = element.get_array().value();
  if (array.size() != count)
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.temperature_2m_max array has an unexpected number of elements!");
  }
  {
    std::size_t idx = 0;
    for (const auto& t_max: array)
    {
      if (!t_max.is_double())
      {
        return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: An element of daily.temperature_2m_max is not a floating-point number!");
      }
      data[idx].temperature_max = t_max.get<double>().value();
      ++idx;
    }
  }

  error = daily["temperature_2m_min"].get(element);
  if (error || !element.is_array())
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.temperature_2m_min element is missing or not an array!");
  }
  array = element.get_array().value();
  if (array.size() != count)
  {
    return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: daily.temperature_2m_min array has an unexpected number of elements!");
  }
  {
    std::size_t idx = 0;
    for (const auto& t_min: array)
    {
      if (!t_min.is_double())
      {
        return nonstd::make_unexpected("Open-Meteo request returned invalid JSON: An element of daily.temperature_2m_min is not a floating-point number!");
      }
      data[idx].temperature_min = t_min.get<double>().value();
      ++idx;
    }
  }

  return data;
}

} // namespace
