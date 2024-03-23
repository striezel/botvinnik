/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020, 2022, 2023, 2024  Dirk Stolle

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

#include "../../locate_catch.hpp"
#include <algorithm>
#include <fstream>
#include "../../FileGuard.hpp"
#include "../../../src/botvinnik/plugins/core/Help.hpp"

bool load_test_configuration(bvn::Configuration& conf)
{
  namespace fs = std::filesystem;
  const fs::path path{fs::temp_directory_path() / "load_test.conf"};
  const FileGuard guard{path};
  {
    std::ofstream stream(path);
    stream << "command.prefix=!\n"
           << "matrix.homeserver=https://matrix.example.tld/\n"
           << "matrix.userid=@alice:matrix.example.tld\n"
           << "matrix.password=secret, secret, top(!) secret\n"
           << "bot.stop.allowed.userid=@alice:matrix.example.tld\n"
           << "bot.sync.allowed_failures=12";
    stream.close();
  }
  return conf.load(path.string());
}

TEST_CASE("plugin Help")
{
  using namespace bvn;
  using namespace std::chrono;
  Configuration conf;
  REQUIRE( load_test_configuration(conf) );
  Bot bot(conf);
  Help plugin(bot);

  const auto commands = plugin.commands();

  SECTION("commands must exist")
  {
    REQUIRE_FALSE( commands.empty() );

    // Help command has to be there.
    REQUIRE_FALSE( std::find(commands.begin(), commands.end(), "help") == commands.end() );
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

  SECTION("extended help")
  {
    for (const auto& cmd : commands)
    {
      // Help text must not be empty.
      const Message msg = plugin.helpExtended(cmd, "!");
      REQUIRE_FALSE( msg.body.empty() );
    }
  }

  SECTION("extended help for non-existent command returns no text")
  {
    const Message msg = plugin.helpExtended("plonk", "!");
    REQUIRE( msg.body.empty() );
    REQUIRE( msg.formatted_body.empty() );
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
  }

  SECTION("command handlers must return text for extended help")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const std::string_view mockRoomId = "!AbcDeFgHiJk345:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    REQUIRE( bot.registerPlugin(plugin) );

    // Answer to commands must not be empty.
    auto message = plugin.handleCommand("help", "help !help", mockUserId, mockRoomId, ts);
    REQUIRE_FALSE( message.body.empty() );
    REQUIRE( message.body.find("short explanation for each command") != std::string::npos );

    message = plugin.handleCommand("help", "help help", mockUserId, mockRoomId, ts);
    REQUIRE_FALSE( message.body.empty() );
    REQUIRE( message.body.find("short explanation for each command") != std::string::npos );
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

  SECTION("handler returns error message for extended help of non-existent command")
  {
    const std::string_view mockUserId = "@alice:bob.charlie.tld";
    const std::string_view mockRoomId = "!AbcDeFgHiJk345:bob.charlie.tld";
    const milliseconds ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    auto message = plugin.handleCommand("help", "help !plonk", mockUserId, mockRoomId, ts);
    REQUIRE_FALSE( message.body.empty() );
    REQUIRE( message.body.find("bot does not have a command by the name") != std::string::npos );

    message = plugin.handleCommand("help", "help plonk", mockUserId, mockRoomId, ts);
    REQUIRE_FALSE( message.body.empty() );
    REQUIRE( message.body.find("bot does not have a command by the name") != std::string::npos );
  }

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(plugin) );
  }

  SECTION("allowDeactivation()")
  {
    SECTION("core plugin commands cannot be deactivated")
    {
      for (const auto& cmd : commands)
      {
        REQUIRE_FALSE( plugin.allowDeactivation(cmd) );
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
