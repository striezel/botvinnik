/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2021  Dirk Stolle

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

#include "../locate_catch.hpp"
#include "../../src/botvinnik/plugins/xkcd/XkcdData.hpp"

TEST_CASE("plugin Xkcd: XkcdData retrieval")
{
  using namespace bvn;

  SECTION("getData() for existing comic with transcript")
  {
    const auto comic_data = XkcdData::get(42);
    REQUIRE( comic_data.has_value() );

    const auto data = comic_data.value();

    REQUIRE( data.num == 42 );
    REQUIRE( data.title == "Geico" );
    REQUIRE( data.img == "https://imgs.xkcd.com/comics/geico.jpg" );
    REQUIRE( data.transcript == "I just saved a bunch of money on my car insurance by threatening my agent with a golf club.\n{{title text: David did this}}" );
    REQUIRE( data.alt == "David did this" );
  }

  SECTION("getData() for existing comic without transcript")
  {
    const auto comic_data = XkcdData::get(2444);
    REQUIRE( comic_data.has_value() );

    const auto data = comic_data.value();

    REQUIRE( data.num == 2444 );
    REQUIRE( data.title == "Ingenuity" );
    REQUIRE( data.img == "https://imgs.xkcd.com/comics/ingenuity.png" );
    REQUIRE( data.transcript == "" );
    REQUIRE( data.alt == "Plot twist: Thanks to [mumble mumble] second-order [mumble] Rayleigh-Taylor [mumble] turbulent [mumble] shear, it turns out powered flight is way EASIER on Mars!" );
  }

  SECTION("getData() for non-existing comic")
  {
    const auto comic_data = XkcdData::get(32101);
    REQUIRE_FALSE( comic_data.has_value() );
  }
}
