/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2024, 2025  Dirk Stolle

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
#include "../../../src/botvinnik/plugins/weather/LocationLookup.hpp"

TEST_CASE("plugin Weather: generic location lookup")
{
  using namespace bvn;

  SECTION("find_location")
  {
    SECTION("attempt to find location that does not exist")
    {
      const auto location = LocationLookup::find_location("Waaaaaaargablah");

      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "No matching location was found." );
    }

    SECTION("find existing location")
    {
      const auto location = LocationLookup::find_location("Leipzig");

      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 50.9 );
      REQUIRE( data.latitude <= 51.7 );

      REQUIRE( data.longitude >= 12.0 );
      REQUIRE( data.longitude <= 12.6 );

      REQUIRE( data.name == "Leipzig" );
      const bool match_OSM = data.display_name == "Leipzig, Saxony, Germany";
      const bool match_OpenMeteo = data.display_name == "Leipzig, Saxony, Germany";
      const bool display_name_match = match_OSM || match_OpenMeteo;
      REQUIRE( display_name_match );
    }

    SECTION("find existing location, part 2")
    {
      const auto location = LocationLookup::find_location("Tokyo");

      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 35.5 );
      REQUIRE( data.latitude <= 35.9 );

      REQUIRE( data.longitude >= 139.5 );
      REQUIRE( data.longitude <= 139.9 );

      REQUIRE( data.name == "Tokyo" );
      const bool match_OSM = data.display_name == "Tokyo, Japan";
      const bool match_OpenMeteo = data.display_name == "Tokyo, Tokyo, Japan";
      const bool display_name_match = match_OSM || match_OpenMeteo;
      REQUIRE( display_name_match );
    }
  }
}
