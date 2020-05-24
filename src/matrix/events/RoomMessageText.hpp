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

#ifndef BVN_EVENT_ROOMMESSAGE_TEXT_HPP
#define BVN_EVENT_ROOMMESSAGE_TEXT_HPP

#include <chrono>
#include <string>

namespace bvn::matrix
{

/* Example:

{
    "content":{
        "body":"I've just _texted_ to say I like **Matrix**.",
        "format":"org.matrix.custom.html",
        "formatted_body":"I've just <em>texted</em> to say I like <strong>Matrix</strong>.",
        "msgtype":"m.text"
    },
    "origin_server_ts":1321059413260,
    "sender":"@charlie:three.example.com",
    "type":"m.room.message",
    "unsigned":{
        "age":843380
    },
    "event_id":"$3AfCiaFohdXo_-cyjgPdsMD1Arhy5bEksMteFOpcj7E"
}
*/

/** Represents a text message event. */
struct RoomMessageText
{
  std::string body;   /**< textual representation of the message */
  std::string format; /**< format type (optional) */
  std::string formatted_body; /**< formatted body, e. g. HTML (optional) */
  std::string sender; /**< user id that sent the event */
  std::chrono::milliseconds server_ts; /**< timestamp of event */
}; // struct

} // namespace

#endif // BVN_EVENT_ROOMMESSAGE_TEXT_HPP
