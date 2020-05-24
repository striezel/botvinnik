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

#include "Help.hpp"
#include <map>

namespace bvn
{

Help::Help(Bot& b)
: theBot(b)
{
}

std::vector<std::string> Help::commands() const
{
  return { "help" };
}

Message Help::handleCommand(const std::string_view& command, const std::string_view& message)
{
  if (command == "help")
  {
    std::map<std::string, std::string> oneLiners;
    for (const auto& item : theBot.commands)
    {
      oneLiners[item.first] = item.second.get().helpOneLine(item.first);
    }
    const std::string& prefix = theBot.mat.configuration().prefix();
    Message result("The following commands are available:\n", "The following commands are available:<br />");
    for (const auto& item : oneLiners)
    {
      result.body.append(prefix).append(item.first).append(" - ")
            .append(item.second).append("\n");
      result.formatted_body.append("<code>").append(prefix).append(item.first).append("</code>").append(" - ")
            .append(item.second).append("<br />");
    }
    return result;
  }
  else
    // unknown command
    return Message();
}

std::string Help::helpOneLine(const std::string_view& command) const
{
  if (command == "help")
  {
    return "shows short help for available commands";
  }
  else
    return std::string("No help available for command '").append(command) + "'!";
}

} // namespace
