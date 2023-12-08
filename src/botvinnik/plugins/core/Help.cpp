/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022, 2023  Dirk Stolle

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
#include "../../../net/htmlspecialchars.hpp"

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

Message Help::handleCommand(const std::string_view& command,
                            [[maybe_unused]] const std::string_view& message,
                            [[maybe_unused]] const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if (command == "help")
  {
    // extract command, if any
    const auto requested_command = extractRequestedCommand(command, message);
    if (requested_command.empty())
    {
      return oneLineOverview();
    }
    else
    {
      return extendedHelp(requested_command);
    }
  }
  else
    // unknown command
    return Message();
}

std::string_view Help::extractRequestedCommand(const std::string_view& command,
                                               const std::string_view& message) const
{
  auto requested_command = message.substr(command.size());
  const auto first = requested_command.find_first_not_of(" ");
  if (first != std::string_view::npos)
  {
    requested_command.remove_prefix(first);
  }
  const std::string& prefix = theBot.matrix().configuration().prefix();
  if (requested_command.find(prefix) == 0)
  {
    requested_command.remove_prefix(prefix.size());
  }
  return requested_command;
}

Message Help::oneLineOverview() const
{
  using namespace std::string_literals;

  std::map<std::string, std::string> oneLiners;
  for (const auto& item : theBot.commands)
  {
    oneLiners[item.first] = item.second.get().helpOneLine(item.first);
  }
  const std::string& prefix = theBot.matrix().configuration().prefix();
  Message result("The following commands are available:\n", "The following commands are available:<br />");
  for (const auto& item : oneLiners)
  {
    if (!item.second.empty())
    {
      result.body.append(prefix).append(item.first).append(" - ")
            .append(item.second).append("\n");
      result.formatted_body.append("<code>").append(prefix).append(item.first).append("</code>").append(" - ")
            .append(htmlspecialchars(item.second)).append("<br />\n");
    }
    else
    {
      result.body.append(prefix).append(item.first).append(" - no help available\n");
      result.formatted_body.append("<code>").append(prefix).append(item.first).append("</code>")
             .append(" - no help available<br />\n");
    }
  }
  result.body += "\nIf you want to know more about a specific command, "s
               + "add it's name to the `"s.append(prefix) + "help` command. "
               + "For example, `"s.append(prefix) + "help wiki` or `"s
               .append(prefix) + "help "s.append(prefix) + "wiki` will show a "
               + "more detailed help for the `"s.append(prefix) + "wiki` command.";
  result.formatted_body += "<br />\nIf you want to know more about a "s
      + "specific command, add it's name to the <code>"s.append(prefix)
      + "help</code> command. For example, <code>"s.append(prefix)
      + "help wiki</code> or <code>"s.append(prefix) + "help "s.append(prefix)
      + "wiki</code> will show a more detailed help for the <code>"s
      .append(prefix) + "wiki</code> command.";
  return result;
}

Message Help::extendedHelp(const std::string_view requested_command) const
{
  using namespace std::string_literals;

  const auto iter = theBot.commands.find(std::string(requested_command));
  if (iter == theBot.commands.end())
  {
    // Command not found.
    return Message("The bot does not have a command by the name `"s
      .append(requested_command) + "`, so it cannot show any help for it.",
      "The bot does not have a command by the name <code>"s
      .append(requested_command) + "</code>, so it cannot show any help for it.");
  }

  const std::string& prefix = theBot.matrix().configuration().prefix();
  auto msg = iter->second.get().helpExtended(requested_command, prefix);
  if (msg.body.empty())
  {
     return Message("The bot does not have extended help information for the command `"s
      .append(requested_command) + "`, so it cannot show any.",
      "The bot does not have extended help information for the command <code>"s
      .append(requested_command) + "</code>, so it cannot show any.");
  }

  // Add prefix and command name in front of the message.
  msg.body = "`" + prefix + std::string(requested_command) + "` - " + msg.body;
  if (!msg.formatted_body.empty())
  {
    msg.formatted_body = "<code>" + prefix + std::string(requested_command) + "</code> - " + msg.formatted_body;
  }
  return msg;
}

std::string Help::helpOneLine(const std::string_view& command) const
{
  if (command == "help")
  {
    return "shows short help for available commands or detailed help for a specific command";
  }
  else
    return "";
}

Message Help::helpExtended(const std::string_view& command, const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "help")
  {
    return Message("shows a help message containing all available commands and"
      + " a short explanation for each command. It can also show a more "s
      + "detailed help for a specific command. For example, `"s .append(prefix)
      + "help wiki` or `"s.append(prefix) + "help "s.append(prefix)
      + "wiki` will show detailed help for the `!wiki` command.",
      "shows a help message containing all available commands and"
      + " a short explanation for each command. It can also show a more "s
      + "detailed help for a specific command. For example, <code>"s
      .append(prefix) + "help wiki</code> or <code>"s.append(prefix) + "help "s
      .append(prefix) + "wiki</code> will show detailed help for the `!wiki` command.");
  }
  else
    return Message();
}

bool Help::allowDeactivation([[maybe_unused]] const std::string_view& command) const
{
  // This is a core command plugin. Commands may not be disabled.
  return false;
}

} // namespace
