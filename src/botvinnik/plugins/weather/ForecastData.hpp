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

#ifndef BVN_PLUGIN_WEATHER_FORECASTDATA_HPP
#define BVN_PLUGIN_WEATHER_FORECASTDATA_HPP

#include <string>

namespace bvn
{

/// Contains weather forecast data.
struct ForecastData
{
  ForecastData();

  std::string date;        /**< date of the forecast, in ISO format (YYYY-MM-DD) */
  double temperature_min;  /**< minimum temperature in degrees Celsius */
  double temperature_max;  /**< maximum temperature in degrees Celsius */
  int weather_code;        /**< WMO weather code */
}; // struct

} // namespace

#endif // BVN_PLUGIN_WEATHER_FORECASTDATA_HPP
