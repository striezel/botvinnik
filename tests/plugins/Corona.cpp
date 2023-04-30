/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020, 2022  Dirk Stolle

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

#include "../locate_catch.hpp"
#include <algorithm>
#include <regex>
#include "../../src/botvinnik/Bot.hpp"
#include "../../src/conf/Configuration.hpp"
#include "../../src/botvinnik/plugins/corona/Corona.hpp"

TEST_CASE("plugin Corona")
{
  using namespace bvn;
  using namespace std::chrono;
  Configuration conf;
  Bot bot(conf);
  Corona plugin(bot.matrix());

  const auto commands = plugin.commands();

  SECTION("commands must exist")
  {
    REQUIRE_FALSE( commands.empty() );

    // corona command has to be there.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "corona") == commands.end() );
  }

  SECTION("one line help")
  {
    for (const auto& cmd : commands)
    {
      // Help text must not be empty.
      REQUIRE_FALSE( plugin.helpOneLine(cmd).empty() );
    }
  } // one line help section

  SECTION("one line help for non-existent command returns no text")
  {
    REQUIRE( plugin.helpOneLine("plonk").empty() );
  }

  SECTION("command handlers must return text")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const std::string_view mockRoomId = "!AbcDeFgHiJk345:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    for (const auto& cmd : commands)
    {
      // Answer to commands must not be empty.
      REQUIRE_FALSE( plugin.handleCommand(cmd, cmd, mockUserId, mockRoomId, ts).body.empty() );
    }

    SECTION("check output for a specific country")
    {
      const Message msg = plugin.handleCommand("corona", "corona Germany", mockUserId, mockRoomId, ts);
      REQUIRE_FALSE( msg.body.empty() );
      REQUIRE_FALSE( msg.formatted_body.empty() );
      // must contain country name and country code
      REQUIRE( msg.body.find("Germany") != std::string::npos );
      REQUIRE( msg.formatted_body.find("Germany") != std::string::npos );
      REQUIRE( msg.body.find("DE") != std::string::npos );
      REQUIRE( msg.formatted_body.find("DE") != std::string::npos );
      // Basic data about infection numbers should be present.
      REQUIRE( msg.body.find("infection") != std::string::npos );
      REQUIRE( msg.formatted_body.find("infection") != std::string::npos );
      REQUIRE( msg.body.find("total") != std::string::npos );
      REQUIRE( msg.formatted_body.find("total") != std::string::npos );
      REQUIRE( msg.body.find("cases") != std::string::npos );
      REQUIRE( msg.formatted_body.find("cases") != std::string::npos );
      REQUIRE( msg.body.find("deaths") != std::string::npos );
      REQUIRE( msg.formatted_body.find("deaths") != std::string::npos );
      // Several dates should be in the message.
      const std::regex expr("20[0-9]{2}\\-[0-9]{2}\\-[0-9]{2}");
      std::smatch matches;
      std::regex_search(msg.body, matches, expr, std::regex_constants::match_any);
      // Matches must have been found.
      REQUIRE_FALSE( matches.empty() );
      REQUIRE_FALSE( matches.size() > 3 );

      std::regex_search(msg.formatted_body, matches, expr, std::regex_constants::match_any);
      // Matches must have been found.
      REQUIRE_FALSE( matches.empty() );
      REQUIRE_FALSE( matches.size() > 3 );
    }

    SECTION("check output for Bonaire, Sint Eustatius and Saba")
    {
      for (const std::string country: { "Bonaire", "Sint Eustatius", "Saba" })
      {
        const Message msg = plugin.handleCommand("corona", "corona " + country, mockUserId, mockRoomId, ts);
        REQUIRE_FALSE( msg.body.empty() );
        REQUIRE_FALSE( msg.formatted_body.empty() );
        // must contain country name
        REQUIRE( msg.body.find(country) != std::string::npos );
        REQUIRE( msg.formatted_body.find(country) != std::string::npos );
        // Basic data about infection numbers should be present.
        REQUIRE( msg.body.find("infection") != std::string::npos );
        REQUIRE( msg.formatted_body.find("infection") != std::string::npos );
        REQUIRE( msg.body.find("total") != std::string::npos );
        REQUIRE( msg.formatted_body.find("total") != std::string::npos );
        REQUIRE( msg.body.find("cases") != std::string::npos );
        REQUIRE( msg.formatted_body.find("cases") != std::string::npos );
        REQUIRE( msg.body.find("deaths") != std::string::npos );
        REQUIRE( msg.formatted_body.find("deaths") != std::string::npos );
        // Several dates should be in the message.
        const std::regex expr("20[0-9]{2}\\-[0-9]{2}\\-[0-9]{2}");
        std::smatch matches;
        std::regex_search(msg.body, matches, expr, std::regex_constants::match_any);
        // Matches must have been found.
        REQUIRE_FALSE( matches.empty() );
        REQUIRE_FALSE( matches.size() > 3 );

        std::regex_search(msg.formatted_body, matches, expr, std::regex_constants::match_any);
        // Matches must have been found.
        REQUIRE_FALSE( matches.empty() );
        REQUIRE_FALSE( matches.size() > 3 );
      }
    }
  }

  SECTION("handler returns empty message for non-existent command")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const std::string_view mockRoomId = "!AbcDeFgHiJk345:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    // Answer to commands must be empty.
    REQUIRE( plugin.handleCommand("plonk", "plonk", mockUserId, mockRoomId, ts).body.empty() );
    REQUIRE( plugin.handleCommand("plonk", "plonk", mockUserId, mockRoomId, ts).formatted_body.empty() );
  }

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(plugin) );
  }

  SECTION("allowDeactivation()")
  {
    SECTION("non-core plugin commands can always be deactivated")
    {
      for (const auto& cmd : commands)
      {
        REQUIRE( plugin.allowDeactivation(cmd) );
      }
    }

    SECTION("unknown commands / commands of other plugins cannot be deactivated")
    {
      REQUIRE_FALSE( plugin.allowDeactivation("foo") );
      REQUIRE_FALSE( plugin.allowDeactivation("not-a-command") );
      REQUIRE_FALSE( plugin.allowDeactivation("ping") );
    }
  }
}
