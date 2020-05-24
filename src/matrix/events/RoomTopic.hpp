/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
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

#ifndef BVN_EVENT_ROOMTOPIC_HPP
#define BVN_EVENT_ROOMTOPIC_HPP

#include <chrono>
#include <string>

namespace bvn::matrix
{

/* Example:

{
    "type":"m.room.topic",
    "sender":"@bob:example-server.org",
    "content":{
        "topic":"This is the first room."
    },
    "state_key":"",
    "origin_server_ts":1590034372631,
    "unsigned":{
        "age":225884009
    },
    "event_id":"$aYrV6bBz0DRy4zDawByeSz1eeHU02OM04L9Aim1OstU"
}
*/

/** Represents a room topic change event. */
struct RoomTopic
{
  std::string topic;   /**< topic of the room */
  std::string sender; /**< user id that sent the event */
  std::chrono::milliseconds server_ts; /**< timestamp of event */
}; // struct

} // namespace

#endif // BVN_EVENT_ROOMTOPIC_HPP
