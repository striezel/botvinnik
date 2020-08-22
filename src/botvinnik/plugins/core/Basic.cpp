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

#include "Basic.hpp"
#include "../../../util/GitInfos.hpp"
#include "../../../Version.hpp"

namespace bvn
{

Basic::Basic(Bot& b)
: theBot(b)
{
}

std::vector<std::string> Basic::commands() const
{
  return { "stop", "version", "whoami" };
}

Message Basic::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts)
{
  if (command == "stop")
  {
    if (theBot.stop(userId))
    {
      return Message(std::string("Stop of bot was requested by ").append(userId)
                     .append(". Shutdown will be initiated."),
                     std::string("<strong>Stop of bot was requested by ")
                     .append(userId).append(". Shutdown will be initiated.</strong>"));
    }
    else
    {
      // User is not allowed to stop the bot.
      Message msg(std::string("You have no power here, ").append(userId)
                  .append("! Only the following users are allowed to stop the bot:\n"),
                  std::string("<strong>You have no power here, ").append(userId)
                  .append("!</strong> Only the following users are allowed to stop the bot:<br />\n"));
      for (const auto& item: theBot.mat.configuration().stopUsers())
      {
        msg.body.append("\n").append(item);
        msg.formatted_body.append("<br />\n").append(item);
      }
      return msg;
    }
  }
  else if (command == "version")
  {
    GitInfos info;
    return Message("botvinnik, " + bvn::version + "\n"
            + "\n"
            + "Version control commit: " + info.commit() + "\n"
            + "Version control date:   " + info.date());
  }
  else if (command == "whoami")
  {
    return Message(std::string("You are ").append(userId).append("."),
                   std::string("You are <strong>").append(userId).append("</strong>."));
  }
  else
    // unknown command
    return Message();
}

std::string Basic::helpOneLine(const std::string_view& command) const
{
  if (command == "stop")
  {
    return "stops the bot and initiates its shutdown";
  }
  else if (command == "version")
  {
    return "shows the version of the bot";
  }
  else if (command == "whoami")
  {
    return "shows the Matrix user id of the user who issued the command";
  }
  else
    return std::string("No help available for command '").append(command) + "'!";
}

} // namespace
