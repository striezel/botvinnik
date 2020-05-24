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


#ifndef BVN_ROOM_HPP
#define BVN_ROOM_HPP

#include "events/RoomMessageText.hpp"
#include "events/RoomName.hpp"
#include "events/RoomTopic.hpp"

namespace bvn::matrix
{

/** Represents a room in Matrix, or rather its events. */
struct Room
{
  std::string id; /**< id of the room */
  std::vector<RoomMessageText> texts; /**< texts in that room */
  std::vector<RoomName> names; /**< name changes of the room */
  std::vector<RoomTopic> topics; /**< topic changes of the room */
}; // struct

} // namespace

#endif // BVN_ROOM_HPP
