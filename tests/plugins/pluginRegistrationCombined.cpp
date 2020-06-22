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
#include "../../src/botvinnik/plugins/core/Basic.hpp"
#include "../../src/botvinnik/plugins/core/Help.hpp"
#include "../../src/botvinnik/plugins/convert/Conversion.hpp"
#include "../../src/botvinnik/plugins/corona/Corona.hpp"
#include "../../src/botvinnik/plugins/Debian.hpp"
#include "../../src/botvinnik/plugins/Fortune.hpp"
#include "../../src/botvinnik/plugins/Ping.hpp"
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
  // other plugins
  Conversion convert;
  Corona cov;
  Debian deb;
  Fortune fortune;
  Ping ping;
  Wikipedia wiki;
  Xkcd xkcd(bot.matrix());

  SECTION("plugin registration")
  {
    // Plugin registration must be successful.
    REQUIRE( bot.registerPlugin(basic) );
    REQUIRE( bot.registerPlugin(help) );
    REQUIRE( bot.registerPlugin(convert) );
    REQUIRE( bot.registerPlugin(cov) );
    REQUIRE( bot.registerPlugin(deb) );
    REQUIRE( bot.registerPlugin(fortune) );
    REQUIRE( bot.registerPlugin(ping) );
    REQUIRE( bot.registerPlugin(wiki) );
    REQUIRE( bot.registerPlugin(xkcd) );
  }
}
