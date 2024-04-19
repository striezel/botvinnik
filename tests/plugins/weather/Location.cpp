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
#include "../../../src/botvinnik/plugins/weather/Location.hpp"

TEST_CASE("plugin Weather: Location")
{
  using namespace bvn;

  SECTION("constructor")
  {
    Location loc;

    REQUIRE( loc.name.empty() );
    REQUIRE( loc.display_name.empty() );
    // NaN is not equal to itself, so this is another way to test for NaN.
    REQUIRE( loc.latitude != loc.latitude );
    REQUIRE( loc.longitude != loc.longitude );
  }

  SECTION("has_data()")
  {
    SECTION("empty")
    {
      Location loc;
      REQUIRE_FALSE( loc.has_data() );
    }

    SECTION("lat. + lon. only")
    {
      Location loc;
      loc.latitude = 5.0;
      loc.longitude = 42.0;
      REQUIRE_FALSE( loc.has_data() );
    }

    SECTION("name only")
    {
      Location loc;
      loc.name = "Fooville";
      REQUIRE_FALSE( loc.has_data() );
    }

    SECTION("name + display name only")
    {
      Location loc;
      loc.name = "Fooville";
      loc.display_name = "Fooville, Bar county, Bazland";
      REQUIRE_FALSE( loc.has_data() );
    }

    SECTION("all data set")
    {
      Location loc;
      loc.name = "Fooville";
      loc.display_name = "Fooville, Bar county, Bazland";
      loc.latitude = 0.0;
      loc.longitude = 42.0;
      REQUIRE( loc.has_data() );
    }
  }
}
