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

#include "Ping.hpp"
#include <chrono>

namespace bvn
{

Ping::Ping()
{
}

std::vector<std::string> Ping::commands() const
{
  return { "ping" };
}

std::string humanReadableDuration(const std::chrono::milliseconds& diff)
{
  if (diff <= std::chrono::milliseconds(1000))
  {
    return std::to_string(diff.count()) + " ms";
  }
  if (diff <= std::chrono::seconds(60))
  {
    return std::to_string(diff.count() / 1000) + " s, " + std::to_string(diff.count() % 1000) + " ms";
  }
  // Slow ping ...
  const auto minutes = diff.count() / 60000;
  const auto seconds = (diff.count() - (60000 * minutes)) / 1000;
  const auto ms = diff.count() - (60000 * minutes) - (1000 * seconds);
  return std::to_string(minutes) + " min, " + std::to_string(seconds) + " s, " + std::to_string(ms) + " ms";
}

Message Ping::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (command == "ping")
  {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - server_ts);
    const auto diffStr = humanReadableDuration(diff);
    return Message(std::string(userId).append(": Ping took ").append(diffStr).append(" to arrive."));
  }

  // unknown command
  return Message();
}

std::string Ping::helpOneLine(const std::string_view& command) const
{
  if (command == "ping")
  {
    return "replies with the time it took the message to reach the bot";
  }

  return std::string();
}

} // namespace
