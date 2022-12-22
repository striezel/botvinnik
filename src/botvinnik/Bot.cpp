/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#include "Bot.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "../util/chrono.hpp"
#include "FailCounter.hpp"

namespace bvn
{

Bot::Bot(const Configuration& conf)
: mat(conf),
  commands({}),
  stopped(false)
{
}

bool Bot::registerPlugin(Plugin& plug)
{
  const auto pluginCommands = plug.commands();
  if (pluginCommands.empty())
    return false;

  // Check sanity + duplicates first.
  for (const std::string& cmd: pluginCommands)
  {
    if (cmd.empty())
    {
      std::cerr << "Error: Empty plugin commands are not allowed!" << std::endl;
      return false;
    }
    if (commands.find(cmd) != commands.end())
    {
      std::cerr << "Error: Command '" << cmd << "' is already registered for "
                << "another plugin!" << std::endl;
      return false;
    }
  }

  // Register commands of plugin.
  for (const std::string& cmd: pluginCommands)
  {
    commands.emplace(cmd, plug);
  }

  return true;
}

bool Bot::handleCommandDeactivations()
{
  const auto& to_deactivate = mat.configuration().deactivatedCommands();
  for(const auto& cmd: to_deactivate)
  {
    const auto iter = commands.find(cmd);
    if (iter == commands.end())
    {
      std::cerr << "Error: No command by the name '" << cmd << "' is active. "
                << "Therefore, it cannot be deactivated." << std::endl;
      return false;
    }
    if (!iter->second.get().allowDeactivation(cmd))
    {
      std::cerr << "Error: Deactivating the command '" << cmd << "' is not "
                << "allowed. This may be due to the fact that this command is "
                << "a core command which is essential to operate the bot "
                << "properly." << std::endl;
      return false;
    }
    std::clog << "Info: Command '" << cmd << "' is deactivated." << std::endl;
    commands.erase(iter);
  }

  return true;
}

void Bot::start()
{
  if (commands.empty())
  {
    std::cout << "Info: No plugins / commands have been registered."
              << " Bot will not start." << std::endl;
    return;
  }

  if (!mat.login())
  {
    std::cerr << "Error: Login on Matrix homeserver failed!" << std::endl;
    return;
  }

  checkServerVersion();

  const auto uploadSize = mat.getUploadLimit();
  if (!uploadSize.has_value())
  {
    std::clog << "Warning: Could not get upload size limit from server." << std::endl;
  }
  else
  {
    if (uploadSize.value() == -1)
    {
      std::clog << "Info: Server did not disclose its upload size limit." << std::endl;
    }
    else
    {
      std::clog << "Info: Server upload size limit is " << uploadSize.value() << " bytes." << std::endl;
    }
  }

  std::string next_batch;
  std::vector<matrix::Room> rooms;
  std::vector<std::string> invites;
  if (!mat.sync(next_batch, rooms, invites, ""))
  {
    std::cerr << "Error: Initial sync request failed!" << std::endl;
    mat.logout();
    return;
  }
  if (next_batch.empty())
  {
    std::cerr << "Error: Initial sync request did not return a 'next_batch' value!" << std::endl;
    mat.logout();
    return;
  }

  const std::string prefix = mat.configuration().prefix();
  FailCounter counter(mat.configuration().allowedFailures());

  while (!stopRequested())
  {
    // Sleep a moment to avoid DOS-ing the server.
    // The sync endpoint is not rate-limited, but we do not have to overdo it.
    std::this_thread::sleep_for(mat.configuration().syncDelay());

    const bool syncSuccess = mat.sync(next_batch, rooms, invites, next_batch);
    counter.next(syncSuccess);
    if (!syncSuccess)
    {
      std::cerr << nowToString() << " Error: Sync request failed!\n";
      std::cerr << nowToString() << " Info: " << counter.count() << " of the maximum allowed "
                << counter.limit() << " requests failed during the last " << counter.N << " requests.\n";
      if (counter.limitExceeded())
      {
        std::cerr << nowToString() << " Error: Failure limit was exceeded, quitting!\n";
        mat.logout();
        return;
      }
      continue;
    }
    else
    {
      std::clog << nowToString() << " Info: Sync request was successful." << std::endl;
    }

    // Iterate over events of all rooms.
    handleRoomEvents(prefix, rooms);

    // Iterate over invites.
    for (const auto & roomId : invites)
    {
      joinRoom(roomId);
    } // for

    if (stopRequested())
    {
      std::clog << nowToString() << " Info: Bot stop was requested, exiting sync loop." << std::endl;
      break;
    }
  }
}

void Bot::checkServerVersion()
{
  const auto version = mat.getSynapseVersion();
  if (!version.has_value())
  {
    return;
  }
  if (version.value() >= "1.62.0")
  {
    std::clog << "Warning: Matrix home server is Synapse 1.62.0 or later.\n"
              << "This versions may possibly cache requests for two minutes by"
              << " default, making the bot very slow to respond. If that is "
              << "the case, the server administrator should lower the setting\n\n"
              << "    sync_response_cache_duration\n\n"
              << "in the home server configuration." << std::endl;
  }
}

void Bot::handleRoomEvents(const std::string& prefix, const std::vector<matrix::Room>& rooms)
{
  // Iterate over events of all rooms.
  for (const auto& room : rooms)
  {
    // The bot only cares for text messages, since only those can contain
    // textual commands.
    for (const matrix::RoomMessageText& msg : room.texts)
    {
      // Check whether text starts with prefix.
      // Since there is no C++20 and starts_with() yet, do it another way.
      if (msg.body.rfind(prefix, 0) == 0)
      {
        // Found it!
        const auto spacePos = msg.body.find(' ', prefix.size());
        const std::string command = (spacePos != std::string::npos)
            ? msg.body.substr(prefix.size(), spacePos - prefix.size())
            : msg.body.substr(prefix.size());
        if (command.empty())
        {
          // Nice try, but there are no empty commands.
          continue;
        }
        const auto iter = commands.find(command);
        if (iter == commands.end())
        {
          // There is no such command.
          mat.sendMessage(room.id, Message("The bot does not recognize the command '" + prefix + command + "'."));
          continue;
        }

        // Let the plugin handle the command.
        const std::string_view message(msg.body.data() + prefix.size(), msg.body.size() - prefix.size());
        const Message answer = iter->second.get().handleCommand(command, message, msg.sender, room.id, msg.server_ts);
        if (!answer.body.empty())
        {
          if (!mat.sendMessage(room.id, answer))
          {
            // Sending messages could fail due to rate limit or because the bot has left the room.
            std::cerr << "Error: Could not send answer for command " + command + "!" << std::endl;
          }
        }
      }
    }
  }
}

void Bot::joinRoom(const std::string& roomId)
{
  if (!mat.joinRoom(roomId))
  {
    std::cerr << "Error: Could not join room " << roomId << "!" << std::endl;
  }
  else
  {
    std::clog << "Info: Joined room " << roomId << "." << std::endl;
    const auto encryption = mat.encryptionAlgorithm(roomId);
    Message leaveMessage;
    if (encryption.has_value() && !encryption.value().empty())
    {
      // Room uses encryption, but the bot cannot handle this yet.
      std::clog << "Info: The room " << roomId << " uses encryption ("
                << encryption.value() << "), but the bot cannot handle "
                << "encrypted messages (yet)." << std::endl;
      leaveMessage = Message("This room uses encryption, but the bot cannot decipher such messages (yet). Therefore, it will leave the room.");
    }
    if (!encryption.has_value())
    {
      std::cerr << "Error: Encryption of the room " << roomId
                << " could not be determined!" << std::endl;
      leaveMessage = Message("This room may use encryption, but the bot cannot decipher encrypted messages (yet). Therefore, it will leave the room.");
    }
    // If the message has a body, then there is a reason to leave the room.
    if (!leaveMessage.body.empty())
    {
      if (!mat.sendMessage(roomId, leaveMessage))
      {
        std::cerr << "Error: Could not send message to indicate why the "
                  << "bot is leaving the room " << roomId
                  << ". Attempting to leave anyway." << std::endl;
      }
      if (!mat.leaveRoom(roomId))
      {
        std::cerr << "Error: Could not leave the encrypted room " << roomId
                  << "!" << std::endl;
      }
      else
      {
        std::clog << "Info: Bot left the encrypted room " << roomId << "."
                  << std::endl;
      }
    }
  } // else
}

bool Bot::stop(const std::string_view& userId)
{
  if (mat.configuration().isAdminUser(std::string(userId)))
  {
    stopped = true;
    return true;
  }

  return false;
}

bool Bot::stopRequested() const
{
  return stopped;
}

Matrix& Bot::matrix()
{
  return mat;
}

} // namespace
