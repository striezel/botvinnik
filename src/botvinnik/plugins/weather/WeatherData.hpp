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

#ifndef BVN_PLUGIN_WEATHER_WEATHERDATA_HPP
#define BVN_PLUGIN_WEATHER_WEATHERDATA_HPP

#include <string>

namespace bvn
{

/// Contains basic weather data.
struct WeatherData
{
  WeatherData();

  double temperature_celsius;  /**< temperature in degrees Celsius */
  double apparent_temperature; /**< apparent temperature in °C */
  int relative_humidity;       /**< relativ humidity in percent */
  int weather_code;            /**< WMO weather code */
  double wind_speed;           /**< wind speed in km/h */
  double wind_direction;       /**< direction of wind in degrees, with 0=N, 90=E, 180=S, 270=W */
  double pressure;             /**< air pressure in hPa */
  double precipitation;        /**< precipitation in millimeters */
}; // struct

} // namespace

#endif // BVN_PLUGIN_WEATHER_WEATHERDATA_HPP
