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
}
