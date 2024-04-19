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
#include "../../../src/botvinnik/plugins/weather/FreeFunctions.hpp"
#include <limits>

TEST_CASE("plugin Weather: free functions")
{
  using namespace bvn::weather;

  SECTION("wmo_code_to_text")
  {
    SECTION("some known codes")
    {
      REQUIRE( wmo_code_to_text(0) == "Clear sky" );
      REQUIRE( wmo_code_to_text(1) == "Mainly clear" );
      REQUIRE( wmo_code_to_text(2) == "Partly cloudy" );
      REQUIRE( wmo_code_to_text(3) == "Overcast" );
      REQUIRE( wmo_code_to_text(45) == "Fog" );
      REQUIRE( wmo_code_to_text(51) == "Light drizzle" );
      REQUIRE( wmo_code_to_text(53) == "Moderate drizzle" );
      REQUIRE( wmo_code_to_text(55) == "Dense drizzle" );
      REQUIRE( wmo_code_to_text(56) == "Light freezing drizzle" );
      REQUIRE( wmo_code_to_text(57) == "Dense freezing drizzle" );
      REQUIRE( wmo_code_to_text(61) == "Slight rain" );
      REQUIRE( wmo_code_to_text(63) == "Moderate rain" );
      REQUIRE( wmo_code_to_text(65) == "Dense rain" );
      REQUIRE( wmo_code_to_text(66) == "Light freezing rain" );
      REQUIRE( wmo_code_to_text(67) == "Heavy freezing rain" );
      REQUIRE( wmo_code_to_text(71) == "Slight snow fall" );
      REQUIRE( wmo_code_to_text(73) == "Moderate snow fall" );
      REQUIRE( wmo_code_to_text(75) == "Heavy snow fall" );
      REQUIRE( wmo_code_to_text(77) == "Snow grains" );
      REQUIRE( wmo_code_to_text(80) == "Slight rain showers" );
      REQUIRE( wmo_code_to_text(81) == "Moderate rain showers" );
      REQUIRE( wmo_code_to_text(82) == "Heavy rain showers" );
      REQUIRE( wmo_code_to_text(85) == "Slight snow showers" );
      REQUIRE( wmo_code_to_text(86) == "Heavy snow showers" );
      REQUIRE( wmo_code_to_text(95) == "Thunderstorm" );
      REQUIRE( wmo_code_to_text(96) == "Thunderstorm with slight hail" );
      REQUIRE( wmo_code_to_text(99) == "Thunderstorm with heavy hail" );
    }

    SECTION("unknown codes")
    {
      REQUIRE( wmo_code_to_text(-1) == "Unknown weather condition (code -1)" );
      REQUIRE( wmo_code_to_text(69) == "Unknown weather condition (code 69)" );
      REQUIRE( wmo_code_to_text(100) == "Unknown weather condition (code 100)" );
    }
  }

  SECTION("wmo_code_to_icon")
  {
    SECTION("some known codes")
    {
      // clear sky
      REQUIRE( wmo_code_to_icon(0) == "\xE2\x98\x80" );
      // Mainly clear
      REQUIRE( wmo_code_to_icon(1) == "\xF0\x9F\x8C\xA4" );
      // Partly cloudy
      REQUIRE( wmo_code_to_icon(2) == "\xF0\x9F\x8C\xA5" );
      // Overcast
      REQUIRE( wmo_code_to_icon(3) == "\xE2\x98\x81" );
      // Fog
      REQUIRE( wmo_code_to_icon(45) == "\xF0\x9F\x8C\xAB" );
      // Light drizzle
      REQUIRE( wmo_code_to_icon(51) == "\xF0\x9F\x8C\xA7" );
      // Moderate drizzle
      REQUIRE( wmo_code_to_icon(53) == "\xF0\x9F\x8C\xA7" );
      // Dense drizzle
      REQUIRE( wmo_code_to_icon(55) == "\xF0\x9F\x8C\xA7" );
      // Light freezing drizzle
      REQUIRE( wmo_code_to_icon(56) == "\xF0\x9F\x8C\xA7" );
      // Dense freezing drizzle
      REQUIRE( wmo_code_to_icon(57) == "\xF0\x9F\x8C\xA7" );
      // Slight rain
      REQUIRE( wmo_code_to_icon(61) == "\xF0\x9F\x8C\xA6" );
      // Moderate rain
      REQUIRE( wmo_code_to_icon(63) == "\xF0\x9F\x8C\xA6" );
      // Dense rain
      REQUIRE( wmo_code_to_icon(65) == "\xF0\x9F\x8C\xA6" );
      // Light freezing rain
      REQUIRE( wmo_code_to_icon(66) == "\xF0\x9F\x8C\xA6" );
      // Heavy freezing rain
      REQUIRE( wmo_code_to_icon(67) == "\xF0\x9F\x8C\xA6" );
      // Slight snow fall
      REQUIRE( wmo_code_to_icon(71) == "\xE2\x9D\x84" );
      // Moderate snow fall
      REQUIRE( wmo_code_to_icon(73) == "\xE2\x9D\x84" );
      // Heavy snow fall
      REQUIRE( wmo_code_to_icon(75) == "\xE2\x9D\x84" );
      // Snow grains
      REQUIRE( wmo_code_to_icon(77) == "\xE2\x9D\x84" );
      // Slight rain showers
      REQUIRE( wmo_code_to_icon(80) == "\xF0\x9F\x8C\xA7" );
      // Moderate rain showers
      REQUIRE( wmo_code_to_icon(81) == "\xF0\x9F\x8C\xA7" );
      // Heavy rain showers
      REQUIRE( wmo_code_to_icon(82) == "\xF0\x9F\x8C\xA7" );
      // Slight snow showers
      REQUIRE( wmo_code_to_icon(85) == "\xF0\x9F\x8C\xA8" );
      // Heavy snow showers
      REQUIRE( wmo_code_to_icon(86) == "\xF0\x9F\x8C\xA8" );
      // Thunderstorm
      REQUIRE( wmo_code_to_icon(95) == "\xF0\x9F\x8C\xA9" );
      // Thunderstorm with slight hail
      REQUIRE( wmo_code_to_icon(96) == "\xF0\x9F\x8C\xA9" );
      // Thunderstorm with heavy hail
      REQUIRE( wmo_code_to_icon(99) == "\xF0\x9F\x8C\xA9" );
    }

    SECTION("unknown codes")
    {
      REQUIRE( wmo_code_to_icon(-1) == "" );
      REQUIRE( wmo_code_to_icon(69) == "" );
      REQUIRE( wmo_code_to_icon(100) == "" );
    }
  }

  SECTION("wind_direction_to_text")
  {
    SECTION("some usual values")
    {
      REQUIRE( wind_direction_to_text(0.0) == "N" );
      REQUIRE( wind_direction_to_text(15.0) == "N" );
      REQUIRE( wind_direction_to_text(30.0) == "NE" );
      REQUIRE( wind_direction_to_text(45.0) == "NE" );
      REQUIRE( wind_direction_to_text(60.0) == "NE" );
      REQUIRE( wind_direction_to_text(75.0) == "E" );
      REQUIRE( wind_direction_to_text(90.0) == "E" );
      REQUIRE( wind_direction_to_text(105.0) == "E" );
      REQUIRE( wind_direction_to_text(120.0) == "SE" );
      REQUIRE( wind_direction_to_text(135.0) == "SE" );
      REQUIRE( wind_direction_to_text(150.0) == "SE" );
      REQUIRE( wind_direction_to_text(165.0) == "S" );
      REQUIRE( wind_direction_to_text(180.0) == "S" );
      REQUIRE( wind_direction_to_text(195.0) == "S" );
      REQUIRE( wind_direction_to_text(210.0) == "SW" );
      REQUIRE( wind_direction_to_text(225.0) == "SW" );
      REQUIRE( wind_direction_to_text(240.0) == "SW" );
      REQUIRE( wind_direction_to_text(255.0) == "W" );
      REQUIRE( wind_direction_to_text(270.0) == "W" );
      REQUIRE( wind_direction_to_text(285.0) == "W" );
      REQUIRE( wind_direction_to_text(300.0) == "NW" );
      REQUIRE( wind_direction_to_text(315.0) == "NW" );
      REQUIRE( wind_direction_to_text(330.0) == "NW" );
      REQUIRE( wind_direction_to_text(345.0) == "N" );
      REQUIRE( wind_direction_to_text(360.0) == "N" );
    }

    SECTION("invalid direction values")
    {
      REQUIRE( wind_direction_to_text(-23.45) == "" );
      REQUIRE( wind_direction_to_text(360.05) == "" );

      const double infinity = std::numeric_limits<double>::infinity();
      REQUIRE( wind_direction_to_text(infinity) == "" );
      REQUIRE( wind_direction_to_text(-infinity) == "" );

      const double nan = std::numeric_limits<double>::quiet_NaN();
      REQUIRE( wind_direction_to_text(nan) == "" );
    }
  }

  SECTION("wind_direction_to_arrow")
  {
    SECTION("some usual values")
    {
      // N
      REQUIRE( wind_direction_to_arrow(0.0) == "\xE2\x86\x93" );
      REQUIRE( wind_direction_to_arrow(15.0) == "\xE2\x86\x93" );
      // NE
      REQUIRE( wind_direction_to_arrow(30.0) == "\xE2\x86\x99" );
      REQUIRE( wind_direction_to_arrow(45.0) == "\xE2\x86\x99" );
      REQUIRE( wind_direction_to_arrow(60.0) == "\xE2\x86\x99" );
      // E
      REQUIRE( wind_direction_to_arrow(75.0) == "\xE2\x86\x90" );
      REQUIRE( wind_direction_to_arrow(90.0) == "\xE2\x86\x90" );
      REQUIRE( wind_direction_to_arrow(105.0) == "\xE2\x86\x90" );
      // SE
      REQUIRE( wind_direction_to_arrow(120.0) == "\xE2\x86\x96" );
      REQUIRE( wind_direction_to_arrow(135.0) == "\xE2\x86\x96" );
      REQUIRE( wind_direction_to_arrow(150.0) == "\xE2\x86\x96" );
      // S
      REQUIRE( wind_direction_to_arrow(165.0) == "\xE2\x86\x91" );
      REQUIRE( wind_direction_to_arrow(180.0) == "\xE2\x86\x91" );
      REQUIRE( wind_direction_to_arrow(195.0) == "\xE2\x86\x91" );
      // SW
      REQUIRE( wind_direction_to_arrow(210.0) == "\xE2\x86\x97" );
      REQUIRE( wind_direction_to_arrow(225.0) == "\xE2\x86\x97" );
      REQUIRE( wind_direction_to_arrow(240.0) == "\xE2\x86\x97" );
      // W
      REQUIRE( wind_direction_to_arrow(255.0) == "\xE2\x86\x92" );
      REQUIRE( wind_direction_to_arrow(270.0) == "\xE2\x86\x92" );
      REQUIRE( wind_direction_to_arrow(285.0) == "\xE2\x86\x92" );
      // NW
      REQUIRE( wind_direction_to_arrow(300.0) == "\xE2\x86\x98" );
      REQUIRE( wind_direction_to_arrow(315.0) == "\xE2\x86\x98" );
      REQUIRE( wind_direction_to_arrow(330.0) == "\xE2\x86\x98" );
      // N
      REQUIRE( wind_direction_to_arrow(345.0) == "\xE2\x86\x93" );
      REQUIRE( wind_direction_to_arrow(360.0) == "\xE2\x86\x93" );
    }

    SECTION("invalid direction values")
    {
      REQUIRE( wind_direction_to_arrow(-23.45) == "" );
      REQUIRE( wind_direction_to_arrow(360.05) == "" );

      const double infinity = std::numeric_limits<double>::infinity();
      REQUIRE( wind_direction_to_arrow(infinity) == "" );
      REQUIRE( wind_direction_to_arrow(-infinity) == "" );

      const double nan = std::numeric_limits<double>::quiet_NaN();
      REQUIRE( wind_direction_to_arrow(nan) == "" );
    }
  }
}
