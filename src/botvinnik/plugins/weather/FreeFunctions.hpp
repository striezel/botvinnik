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

#ifndef BVN_PLUGIN_WEATHER_FREE_FUNCTIONS_HPP
#define BVN_PLUGIN_WEATHER_FREE_FUNCTIONS_HPP

#include <string>

namespace bvn::weather
{

/** \brief Translates known WMO weather codes to text for humans.
 *
 * \param code   the WMO weather code
 * \return Returns a description of the coded weather condition, e. g.
 *         "Clear sky" for code zero.
 */
std::string wmo_code_to_text(const int code);

/** \brief Translates wind direction in degrees to text for humans.
 *
 * \param direction   wind direction in degrees, with 0=N, 90=E, 180=S, 270=W
 * \return Returns a description of the wind direction, e. g. "N" for zero degrees.
 *         Returns an empty string, if direction is not within [0.0;360.0].
 */
std::string wind_direction_to_text(const double direction);

} // namespace

#endif // BVN_PLUGIN_WEATHER_FREE_FUNCTIONS_HPP
