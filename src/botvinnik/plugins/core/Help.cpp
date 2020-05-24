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
#include "../../../util/GitInfos.hpp"
#include "../../../Version.hpp"

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

std::string Help::handleCommand(const std::string_view& command, const std::string_view& message)
{
  if (command == "help")
  {
    std::map<std::string, std::string> oneLiners;
    for (const auto& item : theBot.commands)
    {
      oneLiners[item.first] = item.second.get().helpOneLine(item.first);
    }
    const std::string& prefix = theBot.mat.configuration().prefix();
    std::string text = "The following commands are available:\n";
    for (const auto& item : oneLiners)
    {
      text.append(prefix).append(item.first).append(" - ")
          .append(item.second).append("\n");
    }
    return text;
  }
  else
    // unknown command
    return "";
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
