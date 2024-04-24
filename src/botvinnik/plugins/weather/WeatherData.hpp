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

#include <vector>
#include "CurrentData.hpp"
#include "ForecastData.hpp"

namespace bvn
{

/// Contains current weather data and forecast weather data.
struct WeatherData
{
  WeatherData();

  CurrentData current;  /**< current weather data */
  std::vector<ForecastData> forecast; /**< forecast data */
}; // struct

} // namespace

#endif // BVN_PLUGIN_WEATHER_WEATHERDATA_HPP
