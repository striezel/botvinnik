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

#include <algorithm> // std::find
#include <catch.hpp>
#include "../../src/matrix/json/Sync.hpp"

TEST_CASE("parsing sync events")
{
  using namespace bvn;
  std::string nextBatch;
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
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_9012" );
    // Rooms should be empty (no data in join element).
    REQUIRE( rooms.size() == 0 );
    // Should have been invited into two rooms.
    REQUIRE( invitedRoomIds.size() == 2 );
    // Both room ids should be present as in JSON.
    REQUIRE( std::find(invitedRoomIds.begin(), invitedRoomIds.end(), "!test0r:example.com") != invitedRoomIds.end() );
    REQUIRE( std::find(invitedRoomIds.begin(), invitedRoomIds.end(), "!next0r:ma.tricks.example.tld") != invitedRoomIds.end() );
  }

  SECTION("invites to rooms, without any join element")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_9101112",
    "rooms":{
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
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_9101112" );
    // Rooms should be empty (no join element).
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
    "next_batch":"foobar1234_5678_90",
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
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_90" );
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

  SECTION("messages in rooms, without any invite element")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_90123",
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
            }
        }
    }
    }
    )json";
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_90123" );
    // Invited rooms should be empty (no invite element).
    REQUIRE( invitedRoomIds.size() == 0 );
    // Rooms should be one.
    REQUIRE( rooms.size() == 1 );
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
  }

  SECTION("non-text messages in rooms are ignored")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_9876",
    "rooms":{
        "join":{
            "!roomid5678:example.com":{
               "timeline":{
                    "events":[
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
                            "sender":"@text_user_id:example.org",
                            "origin_server_ts":1223344554321,
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"thinks this is an example emote",
                                "format":"org.matrix.custom.html",
                                "formatted_body":"thinks <b>this</b> is an example emote",
                                "msgtype":"m.emote"
                            },
                            "event_id":"$bbbbbbbbbbbbbb:example.org",
                            "origin_server_ts":1234567890,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@emote_user_id:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"This is an example notice",
                                "format":"org.matrix.custom.html",
                                "formatted_body":"This is an <strong>example</strong> notice",
                                "msgtype":"m.notice"
                            },
                            "event_id":"$cccccccccccccc:example.org",
                            "origin_server_ts":123456789012,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@notice_user_id:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"filename.jpg",
                                "info":{
                                    "h":123,
                                    "mimetype":"image/jpeg",
                                    "size":321023,
                                    "w":456
                                },
                                "msgtype":"m.image",
                                "url":"mxc://example.org/foobarIdWasHere"
                            },
                            "event_id":"$ddddddddddddd:example.org",
                            "origin_server_ts":1234567890123,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@image_user_id:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"open-standards-matter.odt",
                                "filename":"open-standards-matter.odt",
                                "info":{
                                    "mimetype":"application/vnd.oasis.opendocument.text",
                                    "size":54321
                                },
                                "msgtype":"m.file",
                                "url":"mxc://example.org/aaaaaaaaabbbbbbbcccc"
                            },
                            "event_id":"$eeeeeeeeeeeeeeeee:example.org",
                            "origin_server_ts":12345678901234,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@file_user_id:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"Beethoven - Ode an die Freude",
                                "info":{
                                    "duration":2140786,
                                    "mimetype":"audio/ogg",
                                    "size":1563685
                                },
                                "msgtype":"m.audio",
                                "url":"mxc://example.org/freude1234567890"
                            },
                            "event_id":"$fffffffffffffffffff:example.org",
                            "origin_server_ts":123456789012345,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@dj_beathoven:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"Big Ben, London, UK",
                                "geo_uri":"geo:51.5008,0.1247",
                                "info":{
                                    "thumbnail_info":{
                                        "h":300,
                                        "mimetype":"image/jpeg",
                                        "size":46144,
                                        "w":300
                                    },
                                    "thumbnail_url":"mxc://example.org/dingdongbellsareringing"
                                },
                                "msgtype":"m.location"
                            },
                            "event_id":"$gggggggggggggggggg:example.org",
                            "origin_server_ts":123456789012346,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@location_sender_id:example.org",
                            "type":"m.room.message",
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content":{
                                "body":"How to E2EE",
                                "info":{
                                    "duration":2140786,
                                    "h":320,
                                    "mimetype":"video/ogg",
                                    "size":7654321,
                                    "thumbnail_info":{
                                        "h":300,
                                        "mimetype":"image/jpeg",
                                        "size":46144,
                                        "w":300
                                    },
                                    "thumbnail_url":"mxc://example.org/eeeeeeeeeeeee2ee",
                                    "w":480
                                },
                                "msgtype":"m.video",
                                "url":"mxc://example.org/ee222222eeeeee222222eeee"
                            },
                            "event_id":"$hhhhhhhhhhhhhhhhhh:example.org",
                            "origin_server_ts":1432735824653,
                            "room_id":"!roomid5678:example.org",
                            "sender":"@video_sender_id:example.org",
                            "type":"m.room.message",
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
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_9876" );
    // Invited rooms should be empty (no data in invite element).
    REQUIRE( invitedRoomIds.size() == 0 );
    // There should only be one room.
    REQUIRE( rooms.size() == 1 );
    // Id of 1st room should be "!roomid5678:example.com".
    REQUIRE( "!roomid5678:example.com" == rooms[0].id );
    // First room should have one text message only.
    REQUIRE( rooms[0].texts.size() == 1 );
    // Check message content.
    REQUIRE( rooms[0].texts[0].body == "Another text message" );
    REQUIRE( rooms[0].texts[0].format == "org.matrix.custom.html" );
    REQUIRE( rooms[0].texts[0].formatted_body == "<b>Another</b> text message" );
    REQUIRE( rooms[0].texts[0].sender == "@text_user_id:example.org" );
    REQUIRE( rooms[0].texts[0].server_ts == std::chrono::milliseconds(1223344554321) );
  }

  SECTION("room name changes")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar4213_5678_9012",
    "rooms":{
        "join": {
            "!roomid1234:example.com":{
                "timeline":{
                    "events":[
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
                            "sender":"@text_user_id:example.org",
                            "origin_server_ts":13375678901234,
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content": {
                                "name": "New room name"
                            },
                            "event_id": "$aabbbbaaaabbbbaaa:example.org",
                            "origin_server_ts": 13375678901235,
                            "room_id": "!roomid1234:example.org",
                            "sender": "@renamer_id:example.org",
                            "state_key": "",
                            "type": "m.room.name",
                            "unsigned": {
                                "age": 1234
                            }
                        }
                    ],
                    "limited":true,
                    "prev_batch":"p12-123456_0_0"
                }
            }
        },
        "invite":{}
    }
    }
    )json";
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar4213_5678_9012" );
    // Only one room should be present.
    REQUIRE( rooms.size() == 1 );
    // Should have been invited into no rooms.
    REQUIRE( invitedRoomIds.size() == 0 );
    // Room should have one text message only.
    REQUIRE( rooms[0].texts.size() == 1 );
    // Check message content.
    REQUIRE( rooms[0].texts[0].body == "This is an example text message" );
    REQUIRE( rooms[0].texts[0].format == "org.matrix.custom.html" );
    REQUIRE( rooms[0].texts[0].formatted_body == "<b>This is an example text message</b>" );
    REQUIRE( rooms[0].texts[0].sender == "@text_user_id:example.org" );
    REQUIRE( rooms[0].texts[0].server_ts == std::chrono::milliseconds(13375678901234) );
    // Room should also have one name change.
    REQUIRE( rooms[0].names.size() == 1 );
    REQUIRE( rooms[0].topics.size() == 0 );
    // Check name change content.
    REQUIRE( rooms[0].names[0].name == "New room name" );
    REQUIRE( rooms[0].names[0].sender == "@renamer_id:example.org" );
    REQUIRE( rooms[0].names[0].server_ts == std::chrono::milliseconds(13375678901235) );
  }

  SECTION("room topic changes")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_5678_9999",
    "rooms":{
        "join": {
            "!roomid1234:example.com":{
                "timeline":{
                    "events":[
                        {
                            "content":{
                                "body":"Somebody will change the topic soon.",
                                "msgtype":"m.text",
                                "format":"org.matrix.custom.html",
                                "formatted_body":"<b>Somebody</b> will change the topic soon."
                            },
                            "type":"m.room.message",
                            "event_id":"$aaaaaaaaaaaaaaaaa:example.org",
                            "room_id":"!roomid1234:example.com",
                            "sender":"@text_user_id:example.org",
                            "origin_server_ts":13375678901234,
                            "unsigned":{
                                "age":1234
                            }
                        },
                        {
                            "content": {
                                "topic": "Nice topic is 'noice'."
                            },
                            "event_id": "$aabbbbaaaabbbbaaa:example.org",
                            "origin_server_ts": 13375678904444,
                            "room_id": "!roomid1234:example.org",
                            "sender": "@somebody_else:example.org",
                            "state_key": "",
                            "type": "m.room.topic",
                            "unsigned": {
                                "age": 1234
                            }
                        }
                    ],
                    "limited":true,
                    "prev_batch":"p12-123456_0_0"
                }
            }
        },
        "invite":{}
    }
    }
    )json";
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_5678_9999" );
    // Only one room should be present.
    REQUIRE( rooms.size() == 1 );
    // Should have been invited into no rooms.
    REQUIRE( invitedRoomIds.size() == 0 );
    // Room should have one text message only.
    REQUIRE( rooms[0].texts.size() == 1 );
    // Check message content.
    REQUIRE( rooms[0].texts[0].body == "Somebody will change the topic soon." );
    REQUIRE( rooms[0].texts[0].format == "org.matrix.custom.html" );
    REQUIRE( rooms[0].texts[0].formatted_body == "<b>Somebody</b> will change the topic soon." );
    REQUIRE( rooms[0].texts[0].sender == "@text_user_id:example.org" );
    REQUIRE( rooms[0].texts[0].server_ts == std::chrono::milliseconds(13375678901234) );
    // Room should also have one topic change.
    REQUIRE( rooms[0].topics.size() == 1 );
    REQUIRE( rooms[0].names.size() == 0 );
    // Check topic content.
    REQUIRE( rooms[0].topics[0].topic == "Nice topic is 'noice'." );
    REQUIRE( rooms[0].topics[0].sender == "@somebody_else:example.org" );
    REQUIRE( rooms[0].topics[0].server_ts == std::chrono::milliseconds(13375678904444) );
  }

  SECTION("no events, next_batch only")
  {
    const std::string json = R"json(
    {
    "next_batch":"foobar1234_was_here"
    }
    )json";
    REQUIRE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be set properly.
    REQUIRE( nextBatch == "foobar1234_was_here" );
    // No room should be present.
    REQUIRE( rooms.size() == 0 );
    // Should have been invited into no rooms.
    REQUIRE( invitedRoomIds.size() == 0 );
  }

  SECTION("no events, missing next_batch")
  {
    const std::string json = "{ }";
    // Parsing fails, because missing next_batch violates the Matrix specification.
    REQUIRE_FALSE( Sync::parse(json, nextBatch, rooms, invitedRoomIds) );

    // Next batch should be empty.
    REQUIRE( nextBatch.empty() );
    // No room should be present.
    REQUIRE( rooms.size() == 0 );
    // Should have been invited into no rooms.
    REQUIRE( invitedRoomIds.size() == 0 );
  }
}
