/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2021, 2022  Dirk Stolle

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

#include "CheatSheet.hpp"
#include <iostream>
#include "../../net/Curly.hpp"
#include "../../net/htmlspecialchars.hpp"
#include "../../net/url_encode.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

std::vector<std::string> CheatSheet::commands() const
{
  return { "cheat", "cheats" };
}

Message CheatSheet::handleCommand(const std::string_view& command, const std::string_view& message,
                                  [[maybe_unused]] const std::string_view& userId,
                                  [[maybe_unused]] const std::string_view& roomId,
                                  [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if ((command != "cheat") && (command != "cheats"))
  {
    // unknown command
    return Message();
  }

  std::string topic(message.substr(command.size() + 1));
  trim(topic);
  if (topic.size() < 2)
  {
    return Message("Cheat sheet topic to search for must be at least two characters long!");
  }

  std::string encodedTopic;
  try
  {
    encodedTopic = urlencode(topic);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not URL-encode cheat.sh search topic!" << std::endl;
    return Message("Error: Could not get information for topic '" + topic + "'!");
  }


  Curly curl;
  // URL is something like https://cheat.sh/bash?qT, e. g. for topic "bash".
  curl.setURL(std::string("https://cheat.sh/").append(encodedTopic)
              .append("?qT"));
  // Pretend we are curl (which we actually are) via User-Agent. This is
  // necessary, because the server would return HTML code instead of plain
  // text otherwise.
  curl.addHeader("User-Agent: curl/7.77.0");
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to cheat.sh failed!" << std::endl
              << "URL: " << curl.getURL() << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get a cheat sheet for topic '" + topic + "' failed. Server returned unexpected response.");
  }

  return Message(
      response.append("\n\nSource: https://cheat.sh/").append(encodedTopic),
      std::string("<pre>").append(htmlspecialchars(response)).append("</pre>")
          .append("<br />\nSource: https://cheat.sh/").append(encodedTopic));
}

std::string CheatSheet::helpOneLine(const std::string_view& command) const
{
  if (command == "cheat")
  {
    return "displays a cheat sheet for the given Unix program / command";
  }
  if (command == "cheats")
  {
    return "displays a cheat sheet for the given Unix program / command (alias for the cheat command)";
  }

  return std::string();
}

} // namespace
