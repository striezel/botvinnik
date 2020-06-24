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

#include "Rooms.hpp"
#include "../../../util/Strings.hpp"

namespace bvn
{

Rooms::Rooms(Bot& b)
: theBot(b)
{
}

std::vector<std::string> Rooms::commands() const
{
  return { "rooms", "leave" };
}

Message Rooms::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (command == "rooms")
  {
    if (theBot.matrix().configuration().isAdminUser(std::string(userId)))
    {
      std::vector<std::string> rooms;
      if (!theBot.matrix().joinedRooms(rooms))
      {
        return Message("Error: Could not retrieve joined rooms from homeserver.");
      }

      Message msg("The bot is currently member of the following rooms:");
      for (const auto& id : rooms)
      {
        msg.body.append("\n\t").append(id);
        // TODO: Add display name of the room.
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
    else
    {
      // User is not allowed to show the bot's rooms.
      Message msg(std::string("You are not allowed to list active rooms of the bot, ").append(userId)
                  .append(". Only the following users are allowed to do that:\n"),
                  std::string("<strong>You are not allowed to list active rooms of the bot, ").append(userId)
                  .append(".</strong> Only the following users are allowed to do that:<br />\n"));
      for (const auto& item: theBot.mat.configuration().stopUsers())
      {
        msg.body.append("\n").append(item);
        msg.formatted_body.append("<br />\n").append(item);
      }
      return msg;
    }
  }
  else if (command == "leave")
  {
    if (!theBot.matrix().configuration().isAdminUser(std::string(userId)))
    {
      // User is not allowed to make the bot leave rooms.
      Message msg(std::string("You have no power here, ").append(userId)
                  .append(". Only the following users are allowed to make me leave Matrix rooms:\n"),
                  std::string("<strong>You have no power here, ").append(userId)
                  .append(".</strong> Only the following users are allowed to make me leave Matrix rooms:<br />\n"));
      for (const auto& item: theBot.mat.configuration().stopUsers())
      {
        msg.body.append("\n").append(item);
        msg.formatted_body.append("<br />\n").append(item);
      }
      return msg;
    }

    std::string roomId(message.substr(command.size()));
    trim(roomId);
    if (roomId.empty())
    {
      return Message(std::string("Hint: You have to enter the Matrix room id after the ").append(command).append(" command."));
    }

    // Notify room members.
    theBot.mat.sendMessage(roomId, Message(std::string("Leaving the room due to request by ").append(userId).append(".")));
    // Leave the room.
    if (!theBot.mat.leaveRoom(roomId))
    {
      return Message("Error: Could not leave the room '" + roomId + "'. Are you sure that is a proper Matrix room id?");
    }
    if (!theBot.mat.forgetRoom(roomId))
    {
      return Message("Bot has left the room '" + roomId + "'. However, the call to the /forget client-server API endpoint failed.");
    }
    return Message("Bot has left the room " + roomId + " and has forgotten about it.");
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
