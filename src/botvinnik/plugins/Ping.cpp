/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2022, 2023  Dirk Stolle

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

namespace bvn
{

Ping::Ping(const std::chrono::milliseconds& bot_sync_delay)
: sync_delay(std::max(std::chrono::milliseconds(100), bot_sync_delay))
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
    return std::to_string(diff.count() / 1000) + " s " + std::to_string(diff.count() % 1000) + " ms";
  }
  // Slow ping ...
  const auto minutes = diff.count() / 60000;
  const auto seconds = (diff.count() - (60000 * minutes)) / 1000;
  const auto ms = diff.count() - (60000 * minutes) - (1000 * seconds);
  return std::to_string(minutes) + " min " + std::to_string(seconds) + " s " + std::to_string(ms) + " ms";
}

Message Ping::handleCommand(const std::string_view& command,
                            [[maybe_unused]] const std::string_view& message,
                            const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            const std::chrono::milliseconds& server_ts)
{
  if (command == "ping")
  {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - server_ts);
    const auto diffStr = humanReadableDuration(diff);
    // Everything between 2000 ms may possibly get eaten by network latency, so
    // let's be a bit generous with the estimate here.
    const auto delay = sync_delay.count() <= 2000 ? "two"
                         : std::to_string(std::chrono::ceil<std::chrono::seconds>(sync_delay).count());
    auto msg = Message(std::string(userId).append(": Ping took ").append(diffStr).append(" to arrive.")
                  .append("\nNote that the bot only queries new events approx. every "
                          + delay + " seconds, so a \"ping\" of up to " + delay
                          + " seconds is not unusual."),
                   std::string(userId).append(": Ping took ").append(diffStr).append(" to arrive.<br />\n")
                  .append("<em>Note that the bot only queries new events approx. every "
                          + delay + " seconds, so a \"ping\" of up to " + delay
                          + " seconds is not unusual.</em>"));
    // Add note about sync request caching in Synapse 1.62.0 and later.
    if (diff >= 2 * sync_delay)
    {
      msg.body += std::string("\n\nThis ping seems to be rather high. It may")
               + " be that you are using Synapse 1.62.0 or later as homeserver."
               + " This versions may possibly cache requests for two minutes by"
               + " default, making the bot very slow to respond. If that is "
               + "the case, the server administrator should lower the setting\n\n"
               + "    sync_response_cache_duration\n\n"
               + "in the homeserver configuration.";
      msg.formatted_body += std::string("<br />\n<br />\nThis ping seems to ")
               + "be rather high. It may be that you are using Synapse 1.62.0"
               + " or later as homeserver. This versions may possibly cache "
               + "requests for two minutes by default, making the bot very "
               + "slow to respond. If that is the case, the server "
               + "administrator should lower the setting<br />\n<br />\n"
               + "    sync_response_cache_duration<br />\n<br />\n"
               + "in the homeserver configuration.";
    }
    return msg;
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

Message Ping::helpExtended(const std::string_view& command,
          [[maybe_unused]] const std::string_view& prefix) const
{
   if (command == "ping")
  {
    return Message("shows the time it took the message to reach the bot",
                   "shows the time it took the message to reach the bot");
  }

  return Message();
}

} // namespace
