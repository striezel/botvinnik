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

#include <algorithm> // std::find
#include <catch.hpp>
#include "../../src/matrix/json/Sync.hpp"
#include "../../third-party/simdjson/simdjson.h"

TEST_CASE("parsing sync events")
{
  using namespace bvn;
  simdjson::dom::parser parser;
  std::vector<matrix::Room> rooms;
  std::vector<std::string> invitedRoomIds;

  SECTION("invites to rooms")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_9012",
    "rooms":{
        "join": {},
        "invite":{
            "!test0r:example.com":{
                "invite_state":{
                    "events":[
                        {
                            "sender":"@alice:bob.tld",
                            "type":"m.room.name",
                            "state_key":"",
                            "content":{
                                "name":"The Room Has A Name"
                            }
                        },
                        {
                            "sender":"@alice:bob.tld",
                            "type":"m.room.member",
                            "state_key":"@bob:example.com",
                            "content":{
                                "membership":"invite"
                            }
                        }
                    ]
                }
            },
            "!next0r:ma.tricks.example.tld":{
                "invite_state":{
                    "events":[
                        {
                            "sender":"@alice:bob.tld",
                            "type":"m.room.name",
                            "state_key":"",
                            "content":{
                                "name":"The Room Has Another Name"
                            }
                        },
                        {
                            "sender":"@alice:bob.tld",
                            "type":"m.room.member",
                            "state_key":"@bob:example.com",
                            "content":{
                                "membership":"invite"
                            }
                        }
                    ]
                }
            }
        }
    }
    }
    )json";
    const auto [doc, error] = parser.parse(json);
    REQUIRE_FALSE( error );
    Sync::parse(doc, rooms, invitedRoomIds);

    // Rooms should be empty (no data in join element).
    REQUIRE( rooms.size() == 0 );
    // Should have been invited into two rooms.
    REQUIRE( invitedRoomIds.size() == 2 );
    // Both room ids should be present as in JSON.
    REQUIRE( std::find(invitedRoomIds.begin(), invitedRoomIds.end(), "!test0r:example.com") != invitedRoomIds.end() );
    REQUIRE( std::find(invitedRoomIds.begin(), invitedRoomIds.end(), "!next0r:ma.tricks.example.tld") != invitedRoomIds.end() );
  }

}
