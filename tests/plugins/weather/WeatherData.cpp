/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
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

#include "../../locate_catch.hpp"
#include <cmath>
#include "../../../src/botvinnik/plugins/weather/WeatherData.hpp"

TEST_CASE("plugin Weather: WeatherData")
{
  using namespace bvn;

  SECTION("constructor")
  {
    WeatherData data;

    REQUIRE( std::isnan(data.temperature_celsius) );
    REQUIRE( std::isnan(data.apparent_temperature) );
    REQUIRE( data.relative_humidity == -1 );
    REQUIRE( data.weather_code == -1 );
    REQUIRE( std::isnan(data.wind_speed) );
    REQUIRE( std::isnan(data.wind_direction) );
    REQUIRE( std::isnan(data.pressure) );
    REQUIRE( std::isnan(data.precipitation) );
  }
}
