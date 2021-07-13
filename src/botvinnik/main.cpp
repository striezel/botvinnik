/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021  Dirk Stolle

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
#include "../../third-party/nlohmann/json.hpp"
#include "../../third-party/simdjson/simdjson.h"
#include "../conf/Configuration.hpp"
#include "../matrix/Matrix.hpp"
#include "../net/Curly.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "Bot.hpp"
#include "plugins/core/Basic.hpp"
#include "plugins/core/Help.hpp"
#include "plugins/core/Rooms.hpp"
#include "plugins/convert/Conversion.hpp"
#include "plugins/corona/Corona.hpp"
#include "plugins/Debian.hpp"
#include "plugins/Fortune.hpp"
#include "plugins/Ping.hpp"
#include "plugins/Wikipedia.hpp"
#include "plugins/xkcd/Xkcd.hpp"

void showVersion()
{
  bvn::GitInfos info;
  std::cout << "botvinnik, " << bvn::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << "\n\n"
            << "Libraries:" << std::endl
            << "  * simdjson " << STRINGIFY(SIMDJSON_VERSION)
            << ", using implementation "
            << simdjson::active_implementation->name() << " ("
            << simdjson::active_implementation->description() << ")" << std::endl
            << "  * nlohmann/json "
            << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR
            << "." << NLOHMANN_JSON_VERSION_PATCH << std::endl;
  const auto ver = Curly::curlVersion();
  std::string data;
  if (!ver.cURL.empty())
  {
    std::cout << "  * curl " << ver.cURL;
    if (!ver.ssl.empty())
    {
      std::cout << " with " << ver.ssl;
      if (!ver.libz.empty())
        std::cout << " and zlib/" << ver.libz;
    }
    else if (!ver.libz.empty())
        std::cout << " with zlib/" << ver.libz;
    std::cout << std::endl;
  }
  else
  {
    std::cout << "  * curl: unknown version" << std::endl;
  }

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

  // load configuration file
  bvn::Configuration config;
  if (!config.load(configurationFile))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return bvn::rcConfigurationError;
  }

  /* Bot currently starts an endless loop that cannot be interrupted by
     the user, yet. Future versions might use something like signal handling
     (as in SIGINT or SIGTERM) to stop the collection.

     The regular way to stop the bot is to send the "!stop" command via chat.
  */
  bvn::Bot bot(config);
  bvn::Basic piBasic(bot);
  if (!bot.registerPlugin(piBasic))
  {
    // Should never happen!
    std::cerr << "Error: Registration of basic plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Help piHelp(bot);
  if (!bot.registerPlugin(piHelp))
  {
    // Should never happen!
    std::cerr << "Error: Registration of help plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Rooms piRooms(bot.matrix());
  if (!bot.registerPlugin(piRooms))
  {
    // Should never happen!
    std::cerr << "Error: Registration of rooms plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Wikipedia wiki;
  if (!bot.registerPlugin(wiki))
  {
    // Should never happen!
    std::cerr << "Error: Registration of wiki plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Ping ping;
  if (!bot.registerPlugin(ping))
  {
    // Should never happen!
    std::cerr << "Error: Registration of ping plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Fortune fortune;
  if (!bot.registerPlugin(fortune))
  {
    // Should never happen!
    std::cerr << "Error: Registration of fortune plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Xkcd xkcd(bot.matrix());
  if (!bot.registerPlugin(xkcd))
  {
    // Should never happen!
    std::cerr << "Error: Registration of xkcd plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Debian deb;
  if (!bot.registerPlugin(deb))
  {
    // Should never happen!
    std::cerr << "Error: Registration of Debian plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Corona covid;
  if (!bot.registerPlugin(covid))
  {
    // Should never happen!
    std::cerr << "Error: Registration of plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }
  bvn::Conversion conv;
  if (!bot.registerPlugin(conv))
  {
    // Should never happen!
    std::cerr << "Error: Registration of conversion plugin failed!" << std::endl
              << "The bot will not start." << std::endl;
    return bvn::rcPluginRegistrationError;
  }

  bot.start();
  std::cout << "Done." << std::endl;
  return 0;
}
