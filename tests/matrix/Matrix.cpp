/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2025  Dirk Stolle

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
#include "../FileGuard.hpp"
#include "../WriteConf.hpp"
#include "../../src/matrix/Matrix.hpp"

TEST_CASE("Matrix")
{
  using namespace bvn;
  namespace fs = std::filesystem;

  // Holds the values for a configuration that allows proper login with a test
  // account.
  const std::string correct_config_content = R"conf(
  # Matrix server login settings
  matrix.homeserver=http://localhost:5000
  matrix.userid=@alice:matrix.example.org
  matrix.password=secret password
  # bot management settings
  command.prefix=!
  bot.sync.allowed_failures=24
  bot.stop.allowed.userid=@alice:matrix.example.org
  )conf";

  SECTION("login and logout")
  {
    SECTION("login with wrong credentials fails")
    {
      const fs::path path{fs::temp_directory_path() / "matrix-wrong-login-credentials.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=http://localhost:5000
      matrix.userid=@alice:matrix.example.org
      matrix.password=wrong password here
      # bot management settings
      command.prefix=!
      bot.sync.allowed_failures=24
      bot.stop.allowed.userid=@alice:matrix.example.org
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "http://localhost:5000" );
      REQUIRE( conf.userId() == "@alice:matrix.example.org" );
      REQUIRE( conf.password() == "wrong password here" );

      Matrix matrix(conf);
      REQUIRE_FALSE( matrix.login() );
    }

    SECTION("login with correct credentials succeeds")
    {
      const fs::path path{fs::temp_directory_path() / "matrix-correct-login-credentials.conf"};
      REQUIRE( writeConfiguration(path, correct_config_content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      Matrix matrix(conf);
      // Login shall succeed.
      REQUIRE( matrix.login() );
      // And so shall the logout afterwards.
      REQUIRE( matrix.logout() );
    }

    SECTION("logging in twice fails, even with correct credentials succeeds")
    {
      const fs::path path{fs::temp_directory_path() / "matrix-login-twice-fails.conf"};
      REQUIRE( writeConfiguration(path, correct_config_content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      Matrix matrix(conf);
      // Login shall succeed.
      REQUIRE( matrix.login() );
      // Second login will fail.
      REQUIRE_FALSE( matrix.login() );
      // But the logout still succeeds.
      REQUIRE( matrix.logout() );
    }

    SECTION("logout without previous login still succeeds")
    {
      Configuration conf;
      Matrix matrix(conf);
      // Logout succeeds.
      REQUIRE( matrix.logout() );
    }
  }

  SECTION("sync requests")
  {
    SECTION("sync request without previous login fails")
    {
      Configuration conf;
      Matrix matrix(conf);

      std::string next_batch;
      std::vector<matrix::Room> rooms;
      std::vector<std::string> invites;

      REQUIRE_FALSE( matrix.sync(next_batch, rooms, invites) );
    }

    SECTION("sync request with proper login succeeds")
    {
      const fs::path path{fs::temp_directory_path() / "matrix-sync-with-correct-credentials.conf"};
      REQUIRE( writeConfiguration(path, correct_config_content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      Matrix matrix(conf);
      REQUIRE( matrix.login() );

      std::string next_batch;
      std::vector<matrix::Room> rooms;
      std::vector<std::string> invites;

      // Sync shall succeed.
      REQUIRE( matrix.sync(next_batch, rooms, invites) );
      // ... and next_batch must be set.
      REQUIRE_FALSE( next_batch.empty() );
    }
  }
}
