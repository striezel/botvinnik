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

#ifndef BVN_PLUGIN_WEATHER_LOCATION_LOOKUP_OPENMETEO_HPP
#define BVN_PLUGIN_WEATHER_LOCATION_LOOKUP_OPENMETEO_HPP

#include <string>
#include "../../../../third-party/nonstd/expected.hpp"
#include "Location.hpp"

namespace bvn
{

/** \brief Finds a location by name via the Open-Meteo geocoding API.
 */
class LocationLookupOpenMeteo
{
  public:
    /** \brief Tries to find a location by its name.
     *
     * \param location_name   name of the location to find, e. g. "Berlin"
     * \return Returns the data for the location in case of success.
     *         Returns a string containing an error message in case of failure.
     */
    static nonstd::expected<Location, std::string> find_location(const std::string_view location_name);


    /** \brief Parses the JSON from an API response.
     *
     * \param response    the text of the response (i. e. JSON text)
     * \return Returns the parsed data for the location in case of success.
     *         Returns a string containing an error message in case of failure.
     */
    static nonstd::expected<Location, std::string> parse_response(const std::string& response);
}; // class

} // namespace

#endif // BVN_PLUGIN_WEATHER_LOCATION_LOOKUP_OPENMETEO_HPP
