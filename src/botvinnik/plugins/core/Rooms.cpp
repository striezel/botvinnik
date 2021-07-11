/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
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

#include "Rooms.hpp"
#include "../../../util/Strings.hpp"

namespace bvn
{

Rooms::Rooms(Matrix& matrix)
: theMatrix(matrix)
{
}

std::vector<std::string> Rooms::commands() const
{
  return { "rooms", "leave" };
}

Message Rooms::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts)
{
  if (command == "rooms")
  {
    if (!theMatrix.configuration().isAdminUser(std::string(userId)))
    {
      // User is not allowed to show the bot's rooms.
      Message msg(std::string("You are not allowed to list active rooms of the bot, ").append(userId)
                  .append(". Only the following users are allowed to do that:\n"),
                  std::string("<strong>You are not allowed to list active rooms of the bot, ").append(userId)
                  .append(".</strong> Only the following users are allowed to do that:<br />\n"));
      for (const auto& item: theMatrix.configuration().stopUsers())
      {
        msg.body.append("\n").append(item);
        msg.formatted_body.append("<br />\n").append(item);
      }
      return msg;
    }

    std::vector<std::string> rooms;
    if (!theMatrix.joinedRooms(rooms))
    {
      return Message("Error: Could not retrieve joined rooms from homeserver.");
    }

    Message msg("The bot is currently member of the following rooms:");
    unsigned int displayNameRequests = 0;
    const unsigned int reqestLimit = 7;
    for (const auto& id : rooms)
    {
      msg.body.append("\n\t").append(id);
      // Add display name when possible.
      if (displayNameRequests < reqestLimit)
      {
        const auto name = theMatrix.roomName(id);
        ++displayNameRequests;
        if (name.has_value() && !name.value().empty())
        {
          msg.body.append(" (\"").append(name.value()).append("\")");
        }
        else if (!name.has_value())
        {
          // Request failed, may be due to rate limit. Avoid further requests.
          displayNameRequests = reqestLimit;
        }
      }
    }
    switch (rooms.size())
    {
      case 0:
           msg.body.append("\nnone");
           break;
      case 1:
           msg.body.append("\nThat is one room only.");
           break;
      default:
           msg.body.append("\nThese are ").append(std::to_string(rooms.size())).append(" rooms in total.");
           break;
    }
    return msg;
  }
  else if (command == "leave")
  {
    std::string leavingRoomId(message.substr(command.size()));
    trim(leavingRoomId);
    if (leavingRoomId.empty())
    {
      // If there is no room id given, attempt to leave the current room.
      leavingRoomId = roomId;
    }

    bool allowed = theMatrix.configuration().isAdminUser(std::string(userId));
    if (!allowed)
    {
      // Check whether user can ban or kick users. If so, bot should leave.
      const auto power = theMatrix.powerLevels(leavingRoomId);
      allowed = power.has_value() && power.value().canBanOrKick(std::string(userId));
    }

    if (!allowed)
    {
      // User is not allowed to make the bot leave rooms.
      Message msg(std::string("You have no power here, ").append(userId)
                  .append(". Only the following users are allowed to make me leave Matrix rooms:\n"),
                  std::string("<strong>You have no power here, ").append(userId)
                  .append(".</strong> Only the following users are allowed to make me leave Matrix rooms:<br />\n"));
      for (const auto& item: theMatrix.configuration().stopUsers())
      {
        msg.body.append("\n").append(item);
        msg.formatted_body.append("<br />\n").append(item);
      }
      msg.body.append("\n\nFurthermore, any user that can ban or kick users from the corresponding room can make the bot leave, too.");
      msg.formatted_body.append("<br />\n<br />\nFurthermore, any user that can ban or kick users from the corresponding room can make the bot leave, too.");
      return msg;
    }

    // Notify room members.
    theMatrix.sendMessage(leavingRoomId, Message(std::string("Leaving the room due to request by ").append(userId).append(".")));
    // Leave the room.
    if (!theMatrix.leaveRoom(leavingRoomId))
    {
      return Message("Error: Could not leave the room '" + leavingRoomId + "'. Are you sure that is a proper Matrix room id?");
    }
    if (!theMatrix.forgetRoom(leavingRoomId))
    {
      return Message("Bot has left the room '" + leavingRoomId + "'. However, the call to the /forget client-server API endpoint failed.");
    }
    return Message("Bot has left the room " + leavingRoomId + " and has forgotten about it.");
  }
  else
    // unknown command
    return Message();
}

std::string Rooms::helpOneLine(const std::string_view& command) const
{
  if (command == "rooms")
  {
    return "shows the rooms where the bot is active";
  }
  else if (command == "leave")
  {
    return "forces the bot to leave the specified room";
  }

  return std::string();
}

} // namespace
