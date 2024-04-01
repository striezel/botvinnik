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

#ifndef BVN_PLUGIN_WEATHER_LOCATION_HPP
#define BVN_PLUGIN_WEATHER_LOCATION_HPP

#include <string>

namespace bvn
{

/// Contains data of a location, e. g. a city or village.
struct Location
{
  Location();


  /** \brief Checks whether all data members are set.
   *
   * \return Returns true, if all data members are set.
   *         Returns false otherwise.
   */
  bool has_data() const;

  double latitude;  /**< latitude of the location in degrees */
  double longitude; /**< longitude of the location in degrees */
  std::string name; /**< short-ish name of the location */
  std::string display_name; /**< long(er) name of the location */
}; // struct

} // namespace

#endif // BVN_PLUGIN_WEATHER_LOCATION_HPP
