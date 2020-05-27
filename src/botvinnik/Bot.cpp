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

#include "Bot.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "../util/chrono.hpp"

namespace bvn
{

Bot::Bot(const Configuration& conf)
: mat(conf),
  commands({}),
  stopped(false)
{
}

Bot::~Bot()
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

bool Bot::deregisterPluginCommand(const std::string& cmd)
{
  if (cmd.empty())
    return false;

  const auto iter = commands.find(cmd);
  if (iter != commands.end())
  {
    commands.erase(iter);
    return true;
  }

  return false;
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

  std::string initialEvents;
  std::string next_batch;
  std::vector<matrix::Room> rooms;
  if (!mat.sync(initialEvents, next_batch, rooms, ""))
  {
    std::cerr << "Error: Initial sync request failed!" << std::endl;
    mat.logout();
    return;
  }

  const std::string prefix = mat.configuration().prefix();

  while (!stopRequested())
  {
    std::string events;
    if (!mat.sync(events, next_batch, rooms, next_batch))
    {
      std::cerr << nowToString() << " Error: Sync request failed!" << std::endl;
      mat.logout();
      return;
    }
    std::clog << nowToString() << " Info: Sync request was successful." << std::endl;

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
          const Message answer = iter->second.get().handleCommand(command, message, msg.sender, msg.server_ts);
          if (!answer.body.empty())
          {
            if (!mat.sendMessage(room.id, answer))
            {
              std::cerr << "Error: Could not send answer for command " + command + "!" << std::endl;
            }
          }
        }
      }
    }

    if (stopRequested())
    {
      std::clog << nowToString() << " Info: Bot stop was requested, exiting sync loop." << std::endl;
      break;
    }

    // Sleep a few seconds to avoid DOS-ing the server.
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

void Bot::stop()
{
  stopped = true;
}

bool Bot::stopRequested() const
{
  return stopped;
}

} // namespace
