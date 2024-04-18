/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2024  Dirk Stolle

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
#include "../../src/Version.hpp"
#include <regex>

TEST_CASE("botvinnik version data")
{
  SECTION("version string")
  {
    REQUIRE_FALSE( bvn::version.empty() );

    // Version contains semver version number.
    const std::regex version_expression("[0-9]+\\.[0-9]+\\.[0-9]+");
    REQUIRE( std::regex_search(bvn::version, version_expression) );

    // Version contains date.
    const std::regex date_expression("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2}");
    REQUIRE( std::regex_search(bvn::version, date_expression) );
  }

  SECTION("user agent string")
  {
    // User agent starts with program name.
    REQUIRE( bvn::userAgent.find("botvinnik") == 0 );

    // Version contains semver version number.
    const std::regex version_expression("[0-9]+\\.[0-9]+\\.[0-9]+");
    REQUIRE( std::regex_search(bvn::userAgent, version_expression) );
  }

  SECTION("version number in user agent and version string match")
  {
    const std::regex version_expression("[0-9]+\\.[0-9]+\\.[0-9]+");

    std::smatch match_version;
    REQUIRE( std::regex_search(bvn::version, match_version, version_expression) );
    REQUIRE( match_version.ready() );

    std::smatch match_agent;
    REQUIRE( std::regex_search(bvn::userAgent, match_agent, version_expression) );
    REQUIRE( match_agent.ready() );

    REQUIRE( match_version.str(0) == match_agent.str(0) );
  }
}
