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
#include "../../../src/botvinnik/plugins/weather/LocationLookupOpenStreetMap.hpp"

TEST_CASE("plugin Weather: location lookup via OpenStreetMap")
{
  using namespace bvn;

  SECTION("attempt to find location that does not exist")
  {
    const auto location = LocationLookup::find_location("Waaaaaaargablah");

    REQUIRE_FALSE( location.has_value() );
    REQUIRE( location.error() == "No matching location was found." );
  }

  SECTION("find existing location")
  {
    const auto location = LocationLookup::find_location("Berlin");

    REQUIRE( location.has_value() );
    const auto data = location.value();

    REQUIRE( data.latitude >= 52.1 );
    REQUIRE( data.latitude <= 52.9 );

    REQUIRE( data.longitude >= 13.1 );
    REQUIRE( data.longitude <= 13.7 );

    REQUIRE( data.name == "Berlin" );
    REQUIRE( data.display_name == "Berlin, Germany" );
  }

  SECTION("find existing location, part 2")
  {
    const auto location = LocationLookup::find_location("Berlin, Camden");

    REQUIRE( location.has_value() );
    const auto data = location.value();

    REQUIRE( data.latitude >= 39.7 );
    REQUIRE( data.latitude <= 39.9 );

    REQUIRE( data.longitude >= -75.1 );
    REQUIRE( data.longitude <= -74.7 );

    REQUIRE( data.name == "Berlin" );
    REQUIRE( data.display_name == "Berlin, Camden County, New Jersey, United States" );
  }
}
