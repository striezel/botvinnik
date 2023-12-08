/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2023  Dirk Stolle

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

#include "Fortune.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <optional>
#include "../../net/htmlspecialchars.hpp"

namespace bvn
{

std::optional<std::string> executeCommand(const char* noArgsCommand)
{
  std::array<char, 512> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(noArgsCommand, "r"), pclose);
  if (!pipe)
  {
    std::cerr << "Error: popen() failed! Maybe the required binary is not installed?" << std::endl;
    return std::optional<std::string>();
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    result += buffer.data();
  }
  return std::optional<std::string>(result);
}

std::vector<std::string> Fortune::commands() const
{
  return { "fortune", "fortunes" };
}

Message Fortune::handleCommand(const std::string_view& command,
                               [[maybe_unused]] const std::string_view& message,
                               [[maybe_unused]] const std::string_view& userId,
                               [[maybe_unused]] const std::string_view& roomId,
                               [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if ((command == "fortune") || (command == "fortunes"))
  {
    // fortune only works when installed.
    const auto text = executeCommand("/usr/games/fortune");
    if (text.has_value() && !text.value().empty())
    {
      return Message(text.value(), std::string("<pre>").append(htmlspecialchars(text.value())).append("</pre>"));
    }

    // fortune may not be installed. Warn user.
    return Message(std::string("Error: Failed to generate fortune. fortune ")
                   + "binary may not be installed. If you are the administrator"
                   + " of the server where the bot runs, then try something like\n\n"
                   + "    apt-get install fortune-mod\n\nor similar to install it.",
                   std::string("<strong>Error: Failed to generate fortune. ")
                   + "fortune binary may not be installed. If you are the administrator"
                   + " of the server where the bot runs, then try something "
                   + "like<br />\n<br />\n<code>"
                   + "apt-get install fortune-mod</code><br />\n<br />\nor "
                   + "similar to install it.");
  }

  // unknown command
  return Message();
}

std::string Fortune::helpOneLine(const std::string_view& command) const
{
  if (command == "fortune")
  {
    return "displays a random epigram / fortune cookie / quote";
  }
  if (command == "fortunes")
  {
    return "displays a random epigram / fortune cookie / quote (alias for the fortune command)";
  }

  return std::string();
}

Message Fortune::helpExtended(const std::string_view& command,
             [[maybe_unused]] const std::string_view& prefix) const
{
  if (command == "fortune")
  {
    return Message("displays a random epigram / fortune cookie / quote");
  }
  if (command == "fortunes")
  {
    return Message("displays a random epigram / fortune cookie / quote (alias for the fortune command)");
  }

  return Message();
}

} // namespace
