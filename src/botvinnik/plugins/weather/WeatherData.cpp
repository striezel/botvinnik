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

#include "WeatherData.hpp"
#include <cmath>
#include <limits>

namespace bvn
{

WeatherData::WeatherData()
: temperature_celsius(std::numeric_limits<double>::quiet_NaN()),
  apparent_temperature(std::numeric_limits<double>::quiet_NaN()),
  relative_humidity(-1),
  weather_code(-1),
  wind_speed(std::numeric_limits<double>::quiet_NaN()),
  wind_direction(std::numeric_limits<double>::quiet_NaN()),
  pressure(std::numeric_limits<double>::quiet_NaN()),
  precipitation(std::numeric_limits<double>::quiet_NaN())
{
}

} // namespace
