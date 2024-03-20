/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2022, 2024  Dirk Stolle

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
#include "../FileGuard.hpp"
#include "../WriteConf.hpp"
#include "../../src/botvinnik/Bot.hpp"
#include "../../src/conf/Configuration.hpp"
#include "../../src/botvinnik/plugins/Ping.hpp"
#include "../../src/botvinnik/plugins/core/Basic.hpp"

TEST_CASE("command deactivation")
{
  using namespace bvn;

  SECTION("deactivation of non-core command is successful")
  {
    const std::filesystem::path path{"deactivating-non-core-command.conf"};
    const std::string content = R"conf(
    # Matrix server login settings
    matrix.homeserver=https://matrix.example.tld/
    matrix.userid=@alice:matrix.example.tld
    matrix.password=secret, secret, top(!) secret
    # bot management settings
    command.prefix=!
    bot.stop.allowed.userid=@alice:matrix.example.tld
    bot.sync.allowed_failures=12
    bot.sync.delay_milliseconds=5000
    # deactivate the !ping command
    command.deactivate=ping
    # translation server settings
    libretranslate.server=https://libretranslate.com
    libretranslate.apikey=abcdef1234567890
    )conf";
    REQUIRE( writeConfiguration(path, content) );
    FileGuard guard{path};

    Configuration conf;
    REQUIRE( conf.load(path.string()) );

    Bot bot(conf);
    Ping plugin(conf.syncDelay());
    REQUIRE( bot.registerPlugin(plugin) );

    REQUIRE( conf.deactivatedCommands().size() == 1 );
    REQUIRE( conf.deactivatedCommands().find("ping") != conf.deactivatedCommands().end() );

    REQUIRE( bot.handleCommandDeactivations() );
  }

  SECTION("deactivation of core command fails")
  {
    const std::filesystem::path path{"deactivating-core-command.conf"};
    const std::string content = R"conf(
    # Matrix server login settings
    matrix.homeserver=https://matrix.example.tld/
    matrix.userid=@alice:matrix.example.tld
    matrix.password=secret, secret, top(!) secret
    # bot management settings
    command.prefix=!
    bot.stop.allowed.userid=@alice:matrix.example.tld
    bot.sync.allowed_failures=12
    bot.sync.delay_milliseconds=5000
    # deactivate the !stop command
    command.deactivate=stop
    # translation server settings
    libretranslate.server=https://libretranslate.com
    libretranslate.apikey=abcdef1234567890
    )conf";
    REQUIRE( writeConfiguration(path, content) );
    FileGuard guard{path};

    Configuration conf;
    REQUIRE( conf.load(path.string()) );

    Bot bot(conf);
    Basic plugin(bot);
    REQUIRE( bot.registerPlugin(plugin) );

    REQUIRE( conf.deactivatedCommands().size() == 1 );
    REQUIRE( conf.deactivatedCommands().find("stop") != conf.deactivatedCommands().end() );

    REQUIRE_FALSE( bot.handleCommandDeactivations() );
  }

  SECTION("deactivation of non-existent command fails")
  {
    const std::filesystem::path path{"deactivating-non-core-command.conf"};
    const std::string content = R"conf(
    # Matrix server login settings
    matrix.homeserver=https://matrix.example.tld/
    matrix.userid=@alice:matrix.example.tld
    matrix.password=secret, secret, top(!) secret
    # bot management settings
    command.prefix=!
    bot.stop.allowed.userid=@alice:matrix.example.tld
    bot.sync.allowed_failures=12
    bot.sync.delay_milliseconds=5000
    # deactivate the !wargarbl command
    command.deactivate=wargarbl
    # translation server settings
    libretranslate.server=https://libretranslate.com
    libretranslate.apikey=abcdef1234567890
    )conf";
    REQUIRE( writeConfiguration(path, content) );
    FileGuard guard{path};

    Configuration conf;
    REQUIRE( conf.load(path.string()) );

    Bot bot(conf);
    Ping plugin(conf.syncDelay());
    REQUIRE( bot.registerPlugin(plugin) );

    REQUIRE( conf.deactivatedCommands().size() == 1 );
    REQUIRE( conf.deactivatedCommands().find("wargarbl") != conf.deactivatedCommands().end() );

    REQUIRE_FALSE( bot.handleCommandDeactivations() );
  }
}
