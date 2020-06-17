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
#include "../../src/botvinnik/plugins/Conversion.hpp"

TEST_CASE("plugin Conversion")
{
  using namespace bvn;
  using namespace std::chrono;
  Configuration conf;
  Bot bot(conf);
  Conversion plugin;

  const auto commands = plugin.commands();

  SECTION("commands must exist")
  {
    REQUIRE_FALSE( commands.empty() );

    // A few conversion commands have to be there.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "bin2dec") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "bin2hex") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "dec2bin") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "dec2hex") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "hex2bin") == commands.end() );
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "hex2dec") == commands.end() );
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
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    for (const auto& cmd : commands)
    {
      // Answer to commands must not be empty.
      REQUIRE_FALSE( plugin.handleCommand(cmd, cmd, mockUserId, ts).body.empty() );
    }
  }

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(plugin) );
  }

  SECTION("conversion test bin2dec")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "bin2dec";
    const std::string_view msg = "bin2dec 110";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("6") != std::string::npos );
  }

  SECTION("conversion test bin2hex")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "bin2hex";
    const std::string_view msg = "bin2hex 11110101";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("f5") != std::string::npos );
  }

  SECTION("conversion test dec2bin")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "dec2bin";
    const std::string_view msg = "dec2bin 123";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("1111011") != std::string::npos );
  }

  SECTION("conversion test dec2hex")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "dec2hex";
    const std::string_view msg = "dec2hex 254";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("fe") != std::string::npos );
  }

  SECTION("conversion test hex2bin")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "hex2bin";
    const std::string_view msg = "hex2bin f5";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("11110101") != std::string::npos );
  }

  SECTION("conversion test hex2dec")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    const std::string_view cmd = "hex2dec";
    const std::string_view msg = "hex2dec fe";
    const Message out = plugin.handleCommand(cmd, msg, mockUserId, ts);

    REQUIRE( out.body.find("254") != std::string::npos );
  }
}
