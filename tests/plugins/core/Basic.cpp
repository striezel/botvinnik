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
#include "../../../src/botvinnik/plugins/core/Basic.hpp"

TEST_CASE("plugin Basic")
{
  using namespace bvn;
  Configuration conf;
  Bot bot(conf);
  Basic plugin(bot);

  const auto commands = plugin.commands();

  SECTION("commands must exist")
  {
    REQUIRE_FALSE( commands.empty() );

    // Stop command has to be there.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "stop") == commands.end() );
    // Version command would be nice to have, too.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "version") == commands.end() );
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
      REQUIRE_FALSE( plugin.handleCommand(cmd, cmd).body.empty() );
    }
  }

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(plugin) );
  }
}
