/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020, 2021  Dirk Stolle

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
#include "../../src/matrix/events/PowerLevels.hpp"
#include "../../src/matrix/json/PowerLevels.hpp"

TEST_CASE("parsing PowerLevels")
{
  using namespace bvn;

  SECTION("example power level event")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must be successful.
    REQUIRE( levels.has_value() );
    // Check levels.
    REQUIRE( 42 == levels.value().ban );
    REQUIRE( 45 == levels.value().kick );
    REQUIRE( 10 == levels.value().users_default );
    // Check users.
    auto iter = levels.value().users.find("@alice:matrix.homeserver.tld");
    REQUIRE_FALSE( iter == levels.value().users.end() );
    REQUIRE( 98 == iter->second );
    iter = levels.value().users.find("@bob:matrix.homeserver.tld");
    REQUIRE_FALSE( iter == levels.value().users.end() );
    REQUIRE( 99 == iter->second );
  }

  SECTION("missing 'ban' element")
  {
    const std::string json = R"json(
{
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": 10
}
    )json";

    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must be successful.
    REQUIRE( levels.has_value() );
    // Required level for ban must default to 50, if it is not given.
    REQUIRE( 50 == levels.value().ban );
  }

  SECTION("missing 'kick' element")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": 10
}
    )json";

    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must be successful.
    REQUIRE( levels.has_value() );
    // Required level for kick must default to 50, if it is not given.
    REQUIRE( 50 == levels.value().kick );
  }

  SECTION("missing 'users_default' element")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    }
}
    )json";

    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must be successful.
    REQUIRE( levels.has_value() );
    // Value of user_defaults must default to zero, if it is not given.
    REQUIRE( 0 == levels.value().users_default );
  }

  SECTION("parsing succeeds even when 'users' is missing")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must be successful.
    REQUIRE( levels.has_value() );
    // Check levels.
    REQUIRE( 42 == levels.value().ban );
    REQUIRE( 45 == levels.value().kick );
    REQUIRE( 10 == levels.value().users_default );
    // Check users.
    REQUIRE( levels.value().users.size() == 0 );
  }

  // The following checks test error handling by giving invalid JSON or wrong
  // types for certain elements.

  SECTION("invalid JSON")
  {
    const std::string json = "{\n\"10\n}";
    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }

  SECTION("invalid: users is not an object")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users": "I am not an object and you should detect it!",
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }

  SECTION("invalid: elements of object 'users' are not numbers")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": false,
        "@charlie:matrix.homeserver.tld": "I am a string!"
    },
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }

  SECTION("invalid: ban is not an integer")
  {
    const std::string json = R"json(
{
    "ban": 42.12,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 45,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }

  SECTION("invalid: kick is not an integer")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": "hey, this is wrong...",
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": 10
}
    )json";


    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }

  SECTION("invalid: users_default is not an integer")
  {
    const std::string json = R"json(
{
    "ban": 42,
    "events": {
        "m.room.avatar": 50,
        "m.room.canonical_alias": 50,
        "m.room.history_visibility": 100,
        "m.room.name": 50,
        "m.room.power_levels": 100
    },
    "events_default": 0,
    "invite": 0,
    "kick": 99,
    "redact": 50,
    "state_default": 50,
    "users": {
        "@alice:matrix.homeserver.tld": 98,
        "@bob:matrix.homeserver.tld": 99
    },
    "users_default": { }
}
    )json";

    const auto levels = bvn::matrix::json::parsePowerLevels(json);
    // Parsing must fail.
    REQUIRE_FALSE( levels.has_value() );
  }
}
