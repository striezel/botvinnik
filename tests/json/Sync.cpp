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

  SECTION("messages in rooms")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_9012",
    "rooms":{
        "join":{
            "!roomid1234:example.com":{
                "summary":{
                    "m.heroes":[
                        "@alice:example.com",
                        "@bob:example.com"
                    ],
                    "m.joined_member_count":2,
                    "m.invited_member_count":0
                },
                "state":{
                    "events":[
                        {
                            "content":{
                                "membership":"join",
                                "avatar_url":"mxc://example.org/SEsfnsuifSDFSSEF",
                                "displayname":"Unit Tests are Good"
                            },
                            "type":"m.room.member",
                            "event_id":"$ashdbasjdhauuuuau:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@example:example.org",
                            "origin_server_ts":12345678901234,
                            "unsigned":{
                                "age":1234
                            },
                            "state_key":"@alice:example.org"
                        }
                    ]
                },
                "timeline":{
                    "events":[
                        {
                            "content":{
                                "membership":"join",
                                "avatar_url":"mxc://example.org/SEsfnsuifSDFSSEF",
                                "displayname":"Unit Tests are Good"
                            },
                            "type":"m.room.member",
                            "event_id":"$ashdbasjdhauuuuau:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@example:example.org",
                            "origin_server_ts":12345678901234,
                            "unsigned":{
                                "age":1234
                            },
                            "state_key":"@alice:example.org"
                        },
                        {
                            "content":{
                                "body":"This is an example text message",
                                "msgtype":"m.text",
                                "format":"org.matrix.custom.html",
                                "formatted_body":"<b>This is an example text message</b>"
                            },
                            "type":"m.room.message",
                            "event_id":"$aaaaaaaaaaaaaaaaa:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@example:example.org",
                            "origin_server_ts":13375678901234,
                            "unsigned":{
                                "age":1234
                            }
                        }
                    ],
                    "limited":true,
                    "prev_batch":"p12-123456_0_0"
                },
                "ephemeral":{
                    "events":[
                        {
                            "content":{
                                "user_ids":[
                                    "@alice:matrix.org",
                                    "@bob:example.com"
                                ]
                            },
                            "type":"m.typing",
                            "room_id":"!09876abcde:example.org"
                        }
                    ]
                }
            },
            "!roomid5678:example.com":{
               "timeline":{
                    "events":[
                        {
                            "content":{
                                "body":"Message without formatted body",
                                "msgtype":"m.text"
                            },
                            "type":"m.room.message",
                            "event_id":"$aaaaaaaaaaaaaaaaa:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@pacman:games.tld",
                            "origin_server_ts":1234567654321,
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"Another text message",
                                "msgtype":"m.text",
                                "format":"org.matrix.custom.html",
                                "formatted_body":"<b>Another</b> text message"
                            },
                            "type":"m.room.message",
                            "event_id":"$aaaaaaaaaaaaaaaaa:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@sending_user_id:example.org",
                            "origin_server_ts":1223344554321,
                            "unsigned":{
                                "age":1234
                            }
                        }
                    ],
                    "limited":true,
                    "prev_batch":"p12-123456_0_0"
                }
            }
        },
        "invite":{ }
    }
    }
    )json";
    const auto [doc, error] = parser.parse(json);
    REQUIRE_FALSE( error );
    Sync::parse(doc, rooms, invitedRoomIds);

    // Invited rooms should be empty (no data in invite element).
    REQUIRE( invitedRoomIds.size() == 0 );
    // Rooms should be two.
    REQUIRE( rooms.size() == 2 );
    // Id of 1st room should be "!roomid1234:example.com".
    REQUIRE( "!roomid1234:example.com" == rooms[0].id );
    // First room should have one text message.
    REQUIRE( rooms[0].texts.size() == 1 );
    // Check contents of 1st message in 1st room.
    REQUIRE( rooms[0].texts[0].body == "This is an example text message" );
    REQUIRE( rooms[0].texts[0].format == "org.matrix.custom.html" );
    REQUIRE( rooms[0].texts[0].formatted_body == "<b>This is an example text message</b>" );
    REQUIRE( rooms[0].texts[0].sender == "@example:example.org" );
    REQUIRE( rooms[0].texts[0].server_ts == std::chrono::milliseconds(13375678901234) );
    // Id of 2nd room should be "!roomid5678:example.com".
    REQUIRE( "!roomid5678:example.com" == rooms[1].id );
    // Second room should have two text messages.
    REQUIRE( rooms[1].texts.size() == 2 );
    // Check contents of 1st message in 2nd room.
    REQUIRE( rooms[1].texts[0].body == "Message without formatted body" );
    REQUIRE( rooms[1].texts[0].format.empty() );
    REQUIRE( rooms[1].texts[0].formatted_body.empty() );
    REQUIRE( rooms[1].texts[0].sender == "@pacman:games.tld" );
    REQUIRE( rooms[1].texts[0].server_ts == std::chrono::milliseconds(1234567654321) );
    // Check contents of 2nd message in 2nd room.
    REQUIRE( rooms[1].texts[1].body == "Another text message" );
    REQUIRE( rooms[1].texts[1].format == "org.matrix.custom.html" );
    REQUIRE( rooms[1].texts[1].formatted_body == "<b>Another</b> text message" );
    REQUIRE( rooms[1].texts[1].sender == "@sending_user_id:example.org" );
    REQUIRE( rooms[1].texts[1].server_ts == std::chrono::milliseconds(1223344554321) );
  }

}
