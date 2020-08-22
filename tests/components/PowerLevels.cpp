/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020  Dirk Stolle

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

#include <catch.hpp>
#include "../../src/matrix/events/PowerLevels.hpp"

TEST_CASE("PowerLevels constructor")
{
  using namespace bvn::matrix;

  SECTION("defaults must be set")
  {
    PowerLevels levels;
    REQUIRE( 50 == levels.ban );
    REQUIRE( 50 == levels.kick );
    REQUIRE( 0 == levels.users_default );
    REQUIRE( 0 == levels.users.size() );
  }
}

TEST_CASE("PowerLevels::canBanOrKick")
{
  using namespace bvn::matrix;

  SECTION("default-constructed PowerLevels")
  {
    PowerLevels levels;

    // Nobody should be able to kick or ban here.
    REQUIRE_FALSE( levels.canBanOrKick("@alice:matrix.homeserver.tld") );
    REQUIRE_FALSE( levels.canBanOrKick("@bob:matrix.homeserver.tld") );
    REQUIRE_FALSE( levels.canBanOrKick("@charlie:matrix.homeserver.tld") );
    REQUIRE_FALSE( levels.canBanOrKick("@dora:matrix.homeserver.tld") );
  }

  SECTION("everyone can ban when default is above ban level")
  {
    PowerLevels levels;
    levels.kick = 80;
    levels.ban = 50;
    levels.users_default = 50;

    // Everyone should be able to kick or ban here.
    REQUIRE( levels.canBanOrKick("@alice:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@bob:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@charlie:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@dora:matrix.homeserver.tld") );
  }

  SECTION("everyone can kick when default is above kick level")
  {
    PowerLevels levels;
    levels.kick = 50;
    levels.ban = 80;
    levels.users_default = 50;

    // Everyone should be able to kick or ban here.
    REQUIRE( levels.canBanOrKick("@alice:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@bob:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@charlie:matrix.homeserver.tld") );
    REQUIRE( levels.canBanOrKick("@dora:matrix.homeserver.tld") );
  }

  SECTION("explicit user levels take precedence over default value, part 1")
  {
    PowerLevels levels;
    levels.kick = 50;
    levels.ban = 50;
    levels.users_default = 100;
    levels.users["@alice:matrix.homeserver.tld"] = 49;

    // Alice should not be able to kick or ban here.
    REQUIRE_FALSE( levels.canBanOrKick("@alice:matrix.homeserver.tld") );
    // Other users should be able to ban or kick.
    REQUIRE( levels.canBanOrKick("@bob:matrix.homeserver.tld") );
  }

  SECTION("explicit user levels take precedence over default value, part 2")
  {
    PowerLevels levels;
    levels.kick = 50;
    levels.ban = 50;
    levels.users_default = 0;
    levels.users["@alice:matrix.homeserver.tld"] = 100;

    // Alice should be able to kick or ban here.
    REQUIRE( levels.canBanOrKick("@alice:matrix.homeserver.tld") );
    // Other users should not be able to ban or kick.
    REQUIRE_FALSE( levels.canBanOrKick("@bob:matrix.homeserver.tld") );
  }
}
