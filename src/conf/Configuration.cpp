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

#include "Configuration.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../util/Directories.hpp"
#include "../util/Strings.hpp"

namespace bvn
{

bool looksLikeUserId(const std::string& str)
{
  // User id must start with "@".

  // std::string::at(0) will not throw, because string is checked for emptiness
  // before it lands here.
  if (str.at(0) != '@')
    return false;
  // User id must contain a ':'.
  return str.find(':') != std::string::npos;
}

// use same comment character as in task files: '#'
const char Configuration::commentCharacter = '#';

Configuration::Configuration()
:
  mHomeServer(""),
  mUserId(""),
  mPassword(""),
  mPrefix(""),
  mStopUsers(std::unordered_set<std::string>()),
  mAllowedFailsIn32(-1)
{
}

std::vector<std::string> Configuration::potentialFileNames()
{
  std::vector<std::string> result = {
      "/etc/botvinnik/botvinnik.conf",
      "/etc/botvinnik/bvn.conf",
      "/etc/bvn/bvn.conf",
      "./botvinnik.conf",
      "./bvn.conf"
  };
  std::string home;
  if (filesystem::getHome(home))
  {
    const auto delim = filesystem::pathDelimiter;
    result.insert(result.begin(), home + delim + ".botvinnik" + delim + "botvinnik.conf");
    result.insert(result.begin(), home + delim + ".botvinnik" + delim + "bvn.conf");
    result.insert(result.begin(), home + delim + ".bvn" + delim + "bvn.conf");
  }
  return result;
}

const std::string& Configuration::homeServer() const
{
  return mHomeServer;
}

const std::string& Configuration::userId() const
{
  return mUserId;
}

const std::string& Configuration::password() const
{
  return mPassword;
}

const std::string& Configuration::prefix() const
{
  return mPrefix;
}

const std::unordered_set<std::string>& Configuration::stopUsers() const
{
  return mStopUsers;
}

bool Configuration::isAdminUser(const std::string& userId) const
{
  return mStopUsers.find(userId) != mStopUsers.end();
}

int Configuration::allowedFailures() const
{
  return mAllowedFailsIn32;
}

void Configuration::findConfigurationFile(std::string& realName)
{
  namespace fs = std::filesystem;

  if (realName.empty())
  {
    for(const auto& file : potentialFileNames())
    {
      const fs::path path(file);
      try
      {
        if (fs::exists(path) && fs::is_regular_file(path))
        {
          realName = file;
          std::cout << "Info: Using configuration file " << file
                    << ", because none has been specified explicitly." << std::endl;
          break;
        }
      }
      catch (const fs::filesystem_error& ex)
      {
        std::cerr << "File system error while checking existence of file "
                  << file << ": " << ex.what() << std::endl;
      } // try-catch
    } // for
  } // if
}

bool Configuration::loadCoreConfiguration(const std::string& fileName)
{
  std::ifstream stream(fileName, std::ios_base::in | std::ios_base::binary);
  if (!stream.good())
  {
    std::cerr << "Error: Could not open configuration file " << fileName << "!" << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(stream, line))
  {
    trim(line);
    // skip empty lines and comment lines
    if (line.empty() || line[0] == commentCharacter)
      continue;

    // check for possible carriage return at end (happens on Windows systems)
    if (line.at(line.length() - 1) == '\r')
    {
      line.erase(line.length() - 1);
    } // if

    const auto sepPos = line.find('=');
    if (sepPos == std::string::npos)
    {
      std::cerr << "Error: Invalid line found: \"" << line <<"\".\n"
                << "General format: \"Name of Setting=value\"" << std::endl;
      return false;
    }
    std::string name = line.substr(0, sepPos);
    trim(name);
    std::string value = line.substr(sepPos);
    value.erase(0, 1);
    trim(value);

    if (value.empty())
    {
      std::cerr << "Error: Value for " << name << " must not be empty!" << std::endl;
      return false;
    }

    if ((name == "matrix.homeserver") || (name == "homeserver"))
    {
      if (!mHomeServer.empty())
      {
        std::cerr << "Error: Matrix home server is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      // Remove trailing slash.
      if (value.size() > 0 && value.at(value.size() - 1) == '/')
        value.erase(value.size() - 1);

      if (value.empty())
      {
        std::cerr << "Error: Host name of Matrix home server must not be empty!" << std::endl;
        return false;
      }

      if (value.substr(0, 7) == "http://")
      {
        std::clog << "Warning: Home server URL does not use HTTPS!" << std::endl;
      }
      else if (value.substr(0, 8) != "https://")
      {
        value = "https://" + value;
        std::cout << "Info: Prepending 'https://' to home server URL, it's now "
                  << value <<"." << std::endl;
      }
      mHomeServer = value;
    } // if matrix.homeserver
    else if ((name == "matrix.userid") || (name == "userid"))
    {
      if (!mUserId.empty())
      {
        std::cerr << "Error: Matrix user id is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (!looksLikeUserId(value))
      {
        std::cerr << "Error: Configuration value of " << name
                  << " does not seem to be a Matrix user id!" << std::endl;
        return false;
      }
      mUserId = value;
    } // if matrix.userid
    else if ((name == "matrix.password") || (name == "password"))
    {
      if (!mPassword.empty())
      {
        std::cerr << "Error: Password is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      mPassword = value;
    } // if matrix.password
    else if ((name == "command.prefix") || (name == "prefix"))
    {
      if (!mPrefix.empty())
      {
        std::cerr << "Error: Command prefix is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      mPrefix = value;
    } // if command.prefix
    else if ((name == "bot.stop.allowed.userid") || (name == "stop.allowed.userid"))
    {
      if (mStopUsers.find(value) != mStopUsers.end())
      {
        std::cerr << "Error: Matrix user id '" << value << "' for user that is"
                  << " allowed to stop the bot is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (!looksLikeUserId(value))
      {
        std::cerr << "Error: Configuration value of " << name
                  << " does not seem to be a Matrix user id!" << std::endl;
        return false;
      }
      mStopUsers.insert(value);
    } // if bot.stop.allowed.userid
    else if ((name == "bot.sync.allowed_failures") || (name == "bot.allowed_failures"))
    {
      if (mAllowedFailsIn32 >= 0)
      {
        std::cerr << "Error: Number of allowed sync failures is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (!stringToInt(value, mAllowedFailsIn32))
      {
        std::cerr << "Error: Number of allowed sync failures in file "
                  << fileName << " must be a non-negative integer!" << std::endl;
        return false;
      }
      // If it is still below zero or above 31, the value is invalid.
      if (mAllowedFailsIn32 < 0 || mAllowedFailsIn32 > 31)
      {
        std::cerr << "Error: Number of allowed sync failures in file "
                  << fileName << " must be between 0 and 31 (inclusive)!" << std::endl;
        return false;
      }
    } // if bot.sync.allowed_failures
    else
    {
      std::cerr << "Error while reading configuration file " << fileName
                << ": There is no setting named \"" << name << "\"!" << std::endl;
      return false;
    } // else (unrecognized setting name)
  } // while

  // Check presence of settings.
  if (mHomeServer.empty() || mUserId.empty() || mPassword.empty())
  {
    std::cerr << "Error: Some settings are missing in configuration file "
              << fileName << ", and thus botvinnik will not be able to work"
              << " properly." << std::endl;
    return false;
  } // if a setting is missing

  if (mStopUsers.empty())
  {
    std::cerr << "Error: bot.stop.allowed.userid has not been specified in "
              << "configuration file " << fileName << "!" << std::endl;
    return false;
  }

  if (mStopUsers.find(mUserId) == mStopUsers.end())
  {
    mStopUsers.insert(mUserId);
    std::clog << "Info: Adding user '" << mUserId << "' to list of users that"
              << " are allowed to stop the bot." << std::endl;
  }

  // Prefix may be missing. Set it to "!" in that case.
  if (mPrefix.empty())
  {
    mPrefix = "!";
    std::clog << "Info: Setting command prefix to '" << mPrefix
              << "', because none is given in the configuration file." << std::endl;
  }
  // Sync fail count may be missing. Set it to 12 in that case.
  if (mAllowedFailsIn32 < 0)
  {
    mAllowedFailsIn32 = 12;
    std::clog << "Info: Setting number of allowed sync failures to " << mAllowedFailsIn32
              << ", because none is given in the configuration file." << std::endl;
  }

  // Everything is good, so far.
  return true;
}

bool Configuration::load(const std::string& fileName)
{
  namespace fs = std::filesystem;

  std::string realName(fileName);
  findConfigurationFile(realName);
  if (realName.empty())
  {
    std::cerr << "Error: No configuration file was found!" << std::endl;
    return false;
  }
  try
  {
    fs::path p(realName);
    if (!fs::exists(p) || !fs::is_regular_file(p))
    {
      std::cerr << "Error: Configuration file " << realName
                << " does not exist or is not a regular file!" << std::endl;
      return false;
    }
  }
  catch (const fs::filesystem_error& ex)
  {
    std::cerr << "File system error while checking existence of file "
              << realName << ": " << ex.what() << std::endl;
  } // try-catch

  // clear any existing information
  clear();

  // load core configuration file
  return loadCoreConfiguration(realName);
}

void Configuration::clear()
{
  mHomeServer.clear();
  mUserId.clear();
  mPassword.clear();
  mStopUsers.clear();
  mAllowedFailsIn32 = -1;
}

} // namespace
