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

#include "../../locate_catch.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include "../../../src/conf/Configuration.hpp"

// utility function to write a configuration file for testing
bool writeConfiguration(const std::filesystem::path& path, const std::string& content)
{
  std::ofstream stream(path, std::ios::out | std::ios::binary);
  if (!stream.good())
    return false;
  if (!stream.write(content.c_str(), content.size()).good())
    return false;
  stream.close();
  return stream.good();
}

// guard to ensure file deletion when it goes out of scope
class FileGuard
{
  private:
    std::filesystem::path path;
  public:
    FileGuard(const std::filesystem::path& filePath)
    : path(filePath)
    { }

    FileGuard(const FileGuard& op) = delete;
    FileGuard(FileGuard&& op) = delete;

    ~FileGuard()
    {
      std::filesystem::remove(path);
    }
};

TEST_CASE("Configuration")
{
  using namespace bvn;

  SECTION("constructor: empty values after construction")
  {
    Configuration conf;
    REQUIRE( conf.homeServer().empty() );
    REQUIRE( conf.userId().empty() );
    REQUIRE( conf.password().empty() );
    REQUIRE( conf.prefix().empty() );
    REQUIRE( conf.stopUsers().empty() );
    REQUIRE( conf.allowedFailures() == -1 );
    REQUIRE( conf.syncDelay() == std::chrono::milliseconds::zero() );
    REQUIRE( conf.translationServer().empty() );
    REQUIRE( conf.translationApiKey().empty() );
  }

  SECTION("comment character must be a printable non-space character")
  {
    const unsigned char cc_as_uchar = static_cast<unsigned char>(Configuration::commentCharacter);

    REQUIRE_FALSE( std::isspace(cc_as_uchar) );
    REQUIRE( std::isprint(cc_as_uchar) );
  }

  SECTION("minimum delay must be above zero, maximum delay must be above minimum")
  {
    REQUIRE( Configuration::min_sync_delay > std::chrono::milliseconds::zero() );
    REQUIRE( Configuration::max_sync_delay > Configuration::min_sync_delay );
    // One minute is already too much time here.
    REQUIRE( std::chrono::minutes(1) > Configuration::max_sync_delay );
  }

  SECTION("potentialFileNames()")
  {
    SECTION("values must not be empty")
    {
      const auto names = Configuration::potentialFileNames();

      REQUIRE_FALSE( names.empty() );
      for (const auto& name: names)
      {
        REQUIRE_FALSE( name.empty() );
      }
    }

    SECTION("values are pairwise distinct")
    {
      const auto names = Configuration::potentialFileNames();
      const auto size = names.size();

      REQUIRE_FALSE( names.empty() );
      for (std::remove_const<decltype(size)>::type i = 0; i < size; ++i)
      {
        for (std::remove_const<decltype(size)>::type j = i + 1; j < size; ++j)
        {
          REQUIRE_FALSE( names[i] == names[j] );
        }
      }
    }
  }

  SECTION("load")
  {
    SECTION("configuration file does not exist")
    {
      Configuration conf;
      REQUIRE_FALSE( conf.load("/this/does/not-exist.conf") );
    }

    SECTION("load example configuration file")
    {
      const std::filesystem::path path{"example-from-documentation.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.example.tld" );
      REQUIRE( conf.userId() == "@alice:matrix.example.tld" );
      REQUIRE( conf.password() == "secret, secret, top(!) secret" );
      REQUIRE( conf.prefix() == "!" );
      REQUIRE( conf.stopUsers().find("@alice:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@bob:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 12 );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(5000) );
      REQUIRE( conf.translationServer() == "https://libretranslate.com" );
      REQUIRE( conf.translationApiKey() == "abcdef1234567890" );
    }

    SECTION("invalid: multiple homeserver values")
    {
      const std::filesystem::path path{"multiple-homeservers.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: missing homeserver value")
    {
      const std::filesystem::path path{"missing-homeserver.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: empty homeserver value")
    {
      const std::filesystem::path path{"empty-homeserver.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("homeserver URL wit HTTP (instead of HTTPS)")
    {
      const std::filesystem::path path{"http-only-homeserver.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=http://matrix.domain.tld
      matrix.userid=@bertie:matrix.domain.tld
      matrix.password=very secret indeed, old chap!
      # bot management settings
      command.prefix=?
      bot.stop.allowed.userid=@jeeves:matrix.domain.tld
      bot.sync.allowed_failures=10
      bot.sync.delay_milliseconds=2345
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "http://matrix.domain.tld" );
      REQUIRE( conf.userId() == "@bertie:matrix.domain.tld" );
      REQUIRE( conf.password() == "very secret indeed, old chap!" );
      REQUIRE( conf.prefix() == "?" );
      REQUIRE( conf.stopUsers().find("@bertie:matrix.domain.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@jeeves:matrix.domain.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 10 );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(2345) );
      REQUIRE( conf.translationServer().empty() );
      REQUIRE( conf.translationApiKey().empty() );
    }

    SECTION("homeserver URL without protocol gets HTTPS protocol")
    {
      const std::filesystem::path path{"homeserver-without-protocol.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=matrix.domain.tld
      matrix.userid=@bertram:matrix.domain.tld
      matrix.password=nothing really here
      # bot management settings
      command.prefix=/
      bot.stop.allowed.userid=@jeeves:matrix.domain.tld
      bot.sync.allowed_failures=5
      bot.sync.delay_milliseconds=12345
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.domain.tld" );
      REQUIRE( conf.userId() == "@bertram:matrix.domain.tld" );
      REQUIRE( conf.password() == "nothing really here" );
      REQUIRE( conf.prefix() == "/" );
      REQUIRE( conf.stopUsers().find("@bertram:matrix.domain.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@jeeves:matrix.domain.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 5 );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(12345) );
      REQUIRE( conf.translationServer().empty() );
      REQUIRE( conf.translationApiKey().empty() );
    }

    SECTION("invalid: multiple user id values")
    {
      const std::filesystem::path path{"multiple-user-ids.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: missing user id value")
    {
      const std::filesystem::path path{"missing-user-id.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: user id is not a Matrix id - domain missing")
    {
      const std::filesystem::path path{"user-id-not-matrix-id-1.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: user id is not a Matrix id - @ is missing")
    {
      const std::filesystem::path path{"user-id-not-matrix-id-2.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: multiple passwords")
    {
      const std::filesystem::path path{"multiple-passwords.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      matrix.password=this should not be here a second time
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: missing password")
    {
      const std::filesystem::path path{"missing-password.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: multiple command prefixes")
    {
      const std::filesystem::path path{"multiple-prefixes.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("missing command prefix becomes default prefix")
    {
      const std::filesystem::path path{"missing-command-prefix.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=top(!) secret
      # bot management settings
      bot.stop.allowed.userid=@alice:matrix.example.tld
      bot.sync.allowed_failures=2
      bot.sync.delay_milliseconds=5000
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.example.tld" );
      REQUIRE( conf.userId() == "@alice:matrix.example.tld" );
      REQUIRE( conf.password() == "top(!) secret" );
      REQUIRE( conf.prefix() == "!" );
      REQUIRE( conf.stopUsers().find("@alice:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 2 );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(5000) );
      REQUIRE( conf.translationServer().empty() );
      REQUIRE( conf.translationApiKey().empty() );
    }

    SECTION("multiple stop user ids are possible")
    {
      const std::filesystem::path path{"multiple-stop-users.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@alice:matrix.example.tld
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.stop.allowed.userid=@charlie:matrix.example.tld
      bot.stop.allowed.userid=@daphne:matrix.example.tld
      bot.sync.allowed_failures=15
      bot.sync.delay_milliseconds=4000
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.example.tld" );
      REQUIRE( conf.userId() == "@alice:matrix.example.tld" );
      REQUIRE( conf.password() == "secret" );
      REQUIRE( conf.prefix() == "!" );
      REQUIRE( conf.stopUsers().find("@alice:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@bob:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@charlie:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@daphne:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 15 );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(4000) );
      REQUIRE( conf.translationServer().empty() );
      REQUIRE( conf.translationApiKey().empty() );
    }

    SECTION("invalid: same stop user id is listed twice")
    {
      const std::filesystem::path path{"same-stop-user-id-twice.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@johndoe:matrix.example.tld
      bot.stop.allowed.userid=@johndoe:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: stop user id is not a matrix id, part 1")
    {
      const std::filesystem::path path{"stop-user-not-a-matrix-id-1.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=bob:matrix.example.tld
      bot.sync.allowed_failures=15
      bot.sync.delay_milliseconds=4000
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: stop user id is not a matrix id, part 2")
    {
      const std::filesystem::path path{"stop-user-not-a-matrix-id-2.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob
      bot.sync.allowed_failures=15
      bot.sync.delay_milliseconds=4000
      # no translation server settings
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: missing stop user id")
    {
      const std::filesystem::path path{"missing-stop-user-id.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("missing allowed failures setting becomes default value")
    {
      const std::filesystem::path path{"missing-allowed-fail-settings.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );
      REQUIRE( conf.allowedFailures() == 12 );
    }

    SECTION("invalid: multiple allowed failures")
    {
      const std::filesystem::path path{"multiple-allowed-fail-settings.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: allowed failures is not an integer")
    {
      const std::filesystem::path path{"allowed-fail-not-an-int.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12what
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: allowed failures is below zero")
    {
      const std::filesystem::path path{"allowed-fail-below-range.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=-1
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: allowed failures is above 31")
    {
      const std::filesystem::path path{"allowed-fail-above-range.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=32
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: multiple sync delay settings")
    {
      const std::filesystem::path path{"multiple-sync-delay-settings.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("missing sync delay setting becomes default value")
    {
      const std::filesystem::path path{"missing-sync-delay-setting.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );
      REQUIRE( conf.syncDelay() == std::chrono::milliseconds(5000) );
    }

    SECTION("invalid: sync delay is not an int")
    {
      const std::filesystem::path path{"sync-delay-not-an-int.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=1.21 gigawatts
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("sync delay below minimum becomes minimum")
    {
      const std::filesystem::path path{"sync-delay-below-minimum.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=50
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.example.tld" );
      REQUIRE( conf.userId() == "@alice:matrix.example.tld" );
      REQUIRE( conf.password() == "secret, secret, top(!) secret" );
      REQUIRE( conf.prefix() == "!" );
      REQUIRE( conf.stopUsers().find("@alice:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@bob:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 12 );
      REQUIRE( conf.syncDelay() == conf.min_sync_delay );
      REQUIRE( conf.translationServer() == "https://libretranslate.com" );
      REQUIRE( conf.translationApiKey() == "abcdef1234567890" );
    }

    SECTION("sync delay above maximum becomes maximum")
    {
      const std::filesystem::path path{"sync-delay-above-maximum.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=15
      bot.sync.delay_milliseconds=34123
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string()) );

      REQUIRE( conf.homeServer() == "https://matrix.example.tld" );
      REQUIRE( conf.userId() == "@alice:matrix.example.tld" );
      REQUIRE( conf.password() == "secret, secret, top(!) secret" );
      REQUIRE( conf.prefix() == "!" );
      REQUIRE( conf.stopUsers().find("@alice:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.stopUsers().find("@bob:matrix.example.tld") != conf.stopUsers().end() );
      REQUIRE( conf.allowedFailures() == 15 );
      REQUIRE( conf.syncDelay() == conf.max_sync_delay );
      REQUIRE( conf.translationServer() == "https://libretranslate.com" );
      REQUIRE( conf.translationApiKey() == "abcdef1234567890" );
    }

    SECTION("invalid: multiple LibreTranslate servers")
    {
      const std::filesystem::path path{"multiple-libretranslate-servers.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: multiple LibreTranslate API keys")
    {
      const std::filesystem::path path{"multiple-libretranslate-keys.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      libretranslate.apikey=abcdef1234567890
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }

    SECTION("invalid: unknown setting name")
    {
      const std::filesystem::path path{"unknown-setting.conf"};
      const std::string content = R"conf(
      # Matrix server login settings
      matrix.homeserver=https://matrix.example.tld/
      matrix.userid=@alice:matrix.example.tld
      matrix.password=secret, secret, top(!) secret
      # bot management settings
      command.prefix=!
      bot.stop.allowed.userid=@bob:matrix.example.tld
      bot.sync.allowed_failures=12
      bot.sync.delay_milliseconds=5000
      # translation server settings
      libretranslate.server=https://libretranslate.com
      libretranslate.apikey=abcdef1234567890
      # What is this?
      oh.no=anyway
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string()) );
    }
  }
}
