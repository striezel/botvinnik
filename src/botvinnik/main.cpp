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

#include <iostream>
#include "../conf/Configuration.hpp"
#include "../matrix/Matrix.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  bvn::GitInfos info;
  std::cout << "botvinnik, " << bvn::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
}

void showHelp()
{
  std::cout << "botvinnik [OPTIONS]\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c FILE | --conf FILE  - sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n";
}

int main(int argc, char** argv)
{
  std::string configurationFile; /**< path of configuration file */

  if ((argc > 1) && (argv != nullptr))
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i] == nullptr)
      {
        std::cerr << "Error: Parameter at index " << i << " is null pointer!\n";
        return bvn::rcInvalidParameter;
      }
      const std::string param(argv[i]);
      if ((param == "-v") || (param == "--version"))
      {
        showVersion();
        return 0;
      } // if version
      else if ((param == "-?") || (param == "/?") || (param == "--help"))
      {
        showHelp();
        return 0;
      } // if help
      else if ((param == "--conf") || (param == "-c"))
      {
        if (!configurationFile.empty())
        {
          std::cerr << "Error: Configuration was already set to "
                    << configurationFile << "!" << std::endl;
          return bvn::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          configurationFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param <<"\"." << std::endl;
          return bvn::rcInvalidParameter;
        }
      } // if configuration file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return bvn::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  // load configuration file + configured tasks
  bvn::Configuration config;
  if (!config.load(configurationFile))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return bvn::rcConfigurationError;
  }

  /* Bot currently starts an endless loop that cannot be interrupted by
     the user, yet. Future versions might use something like signal handling
     (as in SIGINT or SIGTERM) to stop the collection.
  */
  // TODO: Start bot.

  bvn::Matrix mat(config);
  if (mat.login())
  {
    std::cout << "Info: Successfully logged into server.\n";

    std::vector<std::string> rooms;
    if (mat.joinedRooms(rooms))
    {
      std::cout << "Joined rooms (" << rooms.size() << ") are:" << std::endl;
      for(const auto& id : rooms)
      {
        std::cout << "  " << id;
        std::string name;
        if (mat.roomName(id, name))
        {
          std::cout << " (name: " << name << ")";
        }
        std::cout << std::endl;
      }
    }
    else
    {
      std::cerr << "Failed to get joined rooms!" << std::endl;
    }

    std::string allEvents;
    std::string nextBatch;
    if (mat.sync(allEvents, nextBatch))
    {
      std::cout << "Call to sync was successful!" << std::endl
                << "next batch: " << nextBatch << std::endl
                << "all events: " << allEvents << std::endl;
    }
    else
    {
      std::cerr << "Sync failed!" << std::endl;
    }

    if (mat.logout())
    {
      std::cout << "Info: Successfully logged out from server.\n";
    }
    else
    {
      std::cerr << "Error: Logout attempt failed.\n";
    }
  }
  else
  {
    std::cerr << "Error: Login attempt failed. Maybe configuration data is wrong?\n";
  }

  std::cout << "More stuff is not implemented yet." << std::endl;
  std::cout << "Done." << std::endl;
  return 0;
}
