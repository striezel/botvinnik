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

#include "../../locate_catch.hpp"
#include <cctype>
#include <type_traits>
#include "../../../src/conf/Configuration.hpp"

TEST_CASE("Configuration constructor")
{
  using namespace bvn;

  SECTION("empty values after construction")
  {
    Configuration conf;
    REQUIRE( conf.homeServer().empty() );
    REQUIRE( conf.userId().empty() );
    REQUIRE( conf.password().empty() );
    REQUIRE( conf.prefix().empty() );
    REQUIRE( conf.stopUsers().empty() );
    REQUIRE( conf.allowedFailures() == -1 );
    REQUIRE( conf.translationServer().empty() );
    REQUIRE( conf.translationApiKey().empty() );
  }
}

TEST_CASE("Configuration::commentCharacter")
{
  using namespace bvn;

  SECTION("comment character must be a printable non-space character")
  {
    const unsigned char cc_as_uchar = static_cast<unsigned char>(Configuration::commentCharacter);

    REQUIRE_FALSE( std::isspace(cc_as_uchar) );
    REQUIRE( std::isprint(cc_as_uchar) );
  }
}

TEST_CASE("Configuration::potentialFileNames")
{
  using namespace bvn;

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
