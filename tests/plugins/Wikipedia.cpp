/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
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

#include <catch.hpp>
#include <algorithm>
#include "../../src/botvinnik/Bot.hpp"
#include "../../src/conf/Configuration.hpp"
#include "../../src/botvinnik/plugins/Wikipedia.hpp"

TEST_CASE("plugin Wikipedia")
{
  using namespace bvn;
  Configuration conf;
  Bot bot(conf);
  Wikipedia plugin;

  const auto commands = plugin.commands();

  SECTION("commands must exist")
  {
    REQUIRE_FALSE( commands.empty() );

    // wiki commands have to be there.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wiki") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikide") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikien") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikien") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikifr") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikiit") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "wikiru") == commands.end() );
  }

  SECTION("one line help")
  {
    for (const auto& cmd : commands)
    {
      // Help text must not be empty.
      REQUIRE_FALSE( plugin.helpOneLine(cmd).empty() );
    }
  } // one line help section

  SECTION("command handlers must return text")
  {
    for (const auto& cmd : commands)
    {
      // Answer to commands must not be empty.
      const std::string message = cmd + " Einstein";
      REQUIRE_FALSE( plugin.handleCommand(cmd, message).empty() );
    }
  }

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(plugin) );
  }
}
