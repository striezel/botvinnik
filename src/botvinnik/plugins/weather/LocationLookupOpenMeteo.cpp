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

#include "LocationLookupOpenMeteo.hpp"
#include <iostream>
#include "../../../../third-party/simdjson/simdjson.h"
#include "../../../net/Curly.hpp"
#include "../../../net/url_encode.hpp"

namespace bvn
{

nonstd::expected<Location, std::string> LocationLookupOpenMeteo::find_location(const std::string_view location_name)
{
  std::string encoded_location;
  try
  {
    encoded_location = urlencode(location_name);
  }
  catch (const std::exception& ex)
  {
    return nonstd::make_unexpected(ex.what());
  }

  Curly curl;
  curl.setURL("https://geocoding-api.open-meteo.com/v1/search?name=" + encoded_location
              + "&count=1&language=en&format=json");
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

nonstd::expected<Location, std::string> LocationLookupOpenMeteo::parse_response(const std::string& response)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from Open-Meteo!\n"
              << "Response is: " << response << std::endl;
    return nonstd::make_unexpected("The location lookup returned invalid JSON.");
  }

  if (doc.type() != simdjson::dom::element_type::OBJECT)
  {
    return nonstd::make_unexpected("Open-Meteo lookup contained invalid JSON: Root element is not an object!");
  }

  simdjson::dom::element results;
  error = doc.at_pointer("/results").get(results);
  if (error)
  {
    // If no match was found, then the Open-Meteor geocoding API does not
    // generate a results element.
    return nonstd::make_unexpected("No matching location was found.");
  }
  if (results.type() != simdjson::dom::element_type::ARRAY)
  {
    return nonstd::make_unexpected("Open-Meteo lookup contained invalid JSON: results element is not an array!");
  }

  simdjson::dom::element element;
  error = results.at_pointer("/0/name").get(element);
  if (error || element.type() != simdjson::dom::element_type::STRING)
  {
    return nonstd::make_unexpected("Open-Meteo lookup contained invalid JSON: name element is missing or not a string!");
  }
  Location data;
  data.name = element.get<std::string_view>().value();

  // Initialize display name with name, and then fill it up with existing
  // additional information.
  data.display_name = data.name;
  const auto keys_to_search = { "admin1", "country" };
  for (const auto& key: keys_to_search)
  {
    error = results.at_pointer(std::string("/0/") + key).get(element);
    if (!error || element.type() == simdjson::dom::element_type::STRING)
    {
      // Use name and append region data.
      data.display_name.append(", ").append(element.get<std::string_view>().value());
    }
  }

  error = results.at_pointer("/0/latitude").get(element);
  if (error || element.type() != simdjson::dom::element_type::DOUBLE)
  {
    return nonstd::make_unexpected("Open-Meteo lookup contained invalid JSON: latitude is missing or not a floating-point number!");
  }
  data.latitude = element.get<double>().value();

  error = results.at_pointer("/0/longitude").get(element);
  if (error || element.type() != simdjson::dom::element_type::DOUBLE)
  {
    return nonstd::make_unexpected("Open-Meteo lookup contained invalid JSON: longitude is missing or not a floating-point number!");
  }
  data.longitude = element.get<double>().value();

  return data;
}

} // namespace
