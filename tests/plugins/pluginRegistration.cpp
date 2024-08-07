/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020, 2021, 2022, 2023, 2024  Dirk Stolle

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
#include "../../src/botvinnik/Bot.hpp"
#include "../../src/conf/Configuration.hpp"
#include "../../src/botvinnik/plugins/core/Basic.hpp"
#include "../../src/botvinnik/plugins/core/Help.hpp"
#include "../../src/botvinnik/plugins/core/Rooms.hpp"
#include "../../src/botvinnik/plugins/convert/Conversion.hpp"
#include "../../src/botvinnik/plugins/corona/Corona.hpp"
#include "../../src/botvinnik/plugins/CheatSheet.hpp"
#include "../../src/botvinnik/plugins/Debian.hpp"
#include "../../src/botvinnik/plugins/Fortune.hpp"
#include "../../src/botvinnik/plugins/Giphy.hpp"
#include "../../src/botvinnik/plugins/LibreTranslate.hpp"
#include "../../src/botvinnik/plugins/Ping.hpp"
#include "../../src/botvinnik/plugins/weather/Weather.hpp"
#include "../../src/botvinnik/plugins/Wikipedia.hpp"
#include "../../src/botvinnik/plugins/xkcd/Xkcd.hpp"

TEST_CASE("plugin registration for all known plugins")
{
  using namespace bvn;
  using namespace std::chrono;
  Configuration conf;
  Bot bot(conf);
  // core plugins
  Basic basic(bot);
  Help help(bot);
  Rooms rooms(bot.matrix());
  // other plugins
  CheatSheet cheat;
  Conversion convert;
  Corona cov(bot.matrix());
  Debian deb;
  Fortune fortune;
  Giphy giphy("key", bot.matrix());
  LibreTranslate translate("https://libretranslate.com", "");
  Ping ping(std::chrono::milliseconds(2345));
  Weather weather;
  Wikipedia wiki;
  Xkcd xkcd(bot.matrix());

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(basic) );
    REQUIRE( bot.registerPlugin(help) );
    REQUIRE( bot.registerPlugin(rooms) );
    REQUIRE( bot.registerPlugin(cheat) );
    REQUIRE( bot.registerPlugin(convert) );
    REQUIRE( bot.registerPlugin(cov) );
    REQUIRE( bot.registerPlugin(deb) );
    REQUIRE( bot.registerPlugin(fortune) );
    REQUIRE( bot.registerPlugin(giphy) );
    REQUIRE( bot.registerPlugin(translate) );
    REQUIRE( bot.registerPlugin(ping) );
    REQUIRE( bot.registerPlugin(wiki) );
    REQUIRE( bot.registerPlugin(weather) );
    REQUIRE( bot.registerPlugin(xkcd) );
  }
}

TEST_CASE("plugin registration failures")
{
  using namespace bvn;
  Configuration conf;

  SECTION("plugin without commands cannot be registered")
  {
    class NoCommands final : public DeactivatablePlugin
    {
      std::vector<std::string> commands() const override
      {
        return { };
      }

      Message handleCommand([[maybe_unused]] const std::string_view& command,
                            [[maybe_unused]] const std::string_view& message,
                            [[maybe_unused]] const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            [[maybe_unused]] const std::chrono::milliseconds& server_ts) override
      {
        return Message();
      }

      std::string helpOneLine([[maybe_unused]] const std::string_view& command) const override
      {
        return std::string();
      }

      Message helpExtended([[maybe_unused]] const std::string_view& command, [[maybe_unused]] const std::string_view& prefix) const override
      {
        return Message();
      }
    };

    NoCommands no_cmd;

    // Plugin registration must fail.
    Bot bot(conf);
    REQUIRE_FALSE( bot.registerPlugin(no_cmd) );
  }

  SECTION("plugin with empty command name cannot be registered")
  {
    class EmptyCommand : public DeactivatablePlugin
    {
      std::vector<std::string> commands() const override
      {
        return { "" };
      }

      Message handleCommand([[maybe_unused]] const std::string_view& command,
                            [[maybe_unused]] const std::string_view& message,
                            [[maybe_unused]] const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            [[maybe_unused]] const std::chrono::milliseconds& server_ts) override
      {
        return Message();
      }

      std::string helpOneLine([[maybe_unused]] const std::string_view& command) const override
      {
        return std::string();
      }

      Message helpExtended([[maybe_unused]] const std::string_view& command, [[maybe_unused]] const std::string_view& prefix) const override
      {
        return Message();
      }
    };

    EmptyCommand empty_cmd;

    // Plugin registration must fail.
    Bot bot(conf);
    REQUIRE_FALSE( bot.registerPlugin(empty_cmd) );
  }

  SECTION("same command name cannot be registered by two plugins")
  {
    class DoubleCommand final : public DeactivatablePlugin
    {
      std::vector<std::string> commands() const override
      {
        return { "foo", "bar" };
      }

      Message handleCommand([[maybe_unused]] const std::string_view& command,
                            [[maybe_unused]] const std::string_view& message,
                            [[maybe_unused]] const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            [[maybe_unused]] const std::chrono::milliseconds& server_ts) override
      {
        return Message();
      }

      std::string helpOneLine([[maybe_unused]] const std::string_view& command) const override
      {
        return std::string();
      }

      Message helpExtended([[maybe_unused]] const std::string_view& command, [[maybe_unused]] const std::string_view& prefix) const override
      {
        return Message();
      }
    };

    DoubleCommand plugin_one;
    DoubleCommand plugin_two;

    // Plugin registration must fail.
    Bot bot(conf);
    REQUIRE( bot.registerPlugin(plugin_one) );
    REQUIRE_FALSE( bot.registerPlugin(plugin_two) );
  }
}
