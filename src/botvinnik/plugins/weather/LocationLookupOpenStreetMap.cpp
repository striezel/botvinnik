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

#include "LocationLookupOpenStreetMap.hpp"
#include <iostream>
#include "../../../../third-party/simdjson/simdjson.h"
#include "../../../net/Curly.hpp"
#include "../../../net/url_encode.hpp"

namespace bvn
{

nonstd::expected<Location, std::string> LocationLookupOpenStreetMap::find_location(const std::string_view location_name)
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
  curl.setURL("https://nominatim.openstreetmap.org/search?q=" + encoded_location
              + "&accept-language=en&limit=1&format=geojson");
  curl.addHeader("User-Agent: botvinnik/42.0");
  std::string response;
  if (!curl.perform(response))
  {
    std::cerr << "Error: Request to OSM failed!\n"
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return nonstd::make_unexpected("Failed to look up the requested location!");
  }

  return parse_response(response);
}

nonstd::expected<Location, std::string> LocationLookupOpenStreetMap::parse_response(const std::string& response)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from OSM!\n"
              << "Response is: " << response << std::endl;
    return nonstd::make_unexpected("The location lookup returned invalid JSON.");
  }

  if (doc.type() != simdjson::dom::element_type::OBJECT)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: Root element is not an object!");
  }

  simdjson::dom::element features;
  error = doc.at_pointer("/features").get(features);
  if (error || features.type() != simdjson::dom::element_type::ARRAY)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: features element is missing or not an array!");
  }

  if (features.get_array().value().size() == 0)
  {
    return nonstd::make_unexpected("No matching location was found.");
  }

  simdjson::dom::element element;
  error = features.at_pointer("/0/properties/display_name").get(element);
  if (error || element.type() != simdjson::dom::element_type::STRING)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: display_name element is missing or not a string!");
  }
  Location data;
  data.display_name = element.get<std::string_view>().value();

  error = features.at_pointer("/0/properties/name").get(element);
  if (error || element.type() != simdjson::dom::element_type::STRING)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: name element is missing or not a string!");
  }
  data.name = element.get<std::string_view>().value();

  error = features.at_pointer("/0/geometry/coordinates/0").get(element);
  if (error || element.type() != simdjson::dom::element_type::DOUBLE)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: longitude data is missing or not a floating-point number!");
  }
  data.longitude = element.get<double>().value();

  error = features.at_pointer("/0/geometry/coordinates/1").get(element);
  if (error || element.type() != simdjson::dom::element_type::DOUBLE)
  {
    return nonstd::make_unexpected("OSM lookup contained invalid JSON: latitude data is missing or not a floating-point number!");
  }
  data.latitude = element.get<double>().value();

  return data;
}

} // namespace
