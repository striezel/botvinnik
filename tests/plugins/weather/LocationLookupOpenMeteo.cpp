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
#include "../../../src/botvinnik/plugins/weather/LocationLookupOpenMeteo.hpp"

TEST_CASE("plugin Weather: location lookup via Open-Meteo")
{
  using namespace bvn;

  SECTION("attempt to find location that does not exist")
  {
    const auto location = LocationLookupOpenMeteo::find_location("Waaaaaaargablah");

    REQUIRE_FALSE( location.has_value() );
    REQUIRE( location.error() == "No matching location was found." );
  }

  SECTION("find existing location")
  {
    const auto location = LocationLookupOpenMeteo::find_location("Berlin");

    REQUIRE( location.has_value() );
    const auto data = location.value();

    REQUIRE( data.latitude >= 52.1 );
    REQUIRE( data.latitude <= 52.9 );

    REQUIRE( data.longitude >= 13.1 );
    REQUIRE( data.longitude <= 13.7 );

    REQUIRE( data.name == "Berlin" );
    REQUIRE( data.display_name == "Berlin, Land Berlin, Germany" );
  }

  SECTION("find existing location, part 2")
  {
    const auto location = LocationLookupOpenMeteo::find_location("Strasbourg");

    REQUIRE( location.has_value() );
    const auto data = location.value();

    REQUIRE( data.latitude >= 48.3 );
    REQUIRE( data.latitude <= 48.9 );

    REQUIRE( data.longitude >= 7.5 );
    REQUIRE( data.longitude <= 7.9 );

    REQUIRE( data.name == "Strasbourg" );
    REQUIRE( data.display_name == "Strasbourg, Grand Est, France" );
  }
}
