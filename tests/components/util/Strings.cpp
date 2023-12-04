/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020, 2023  Dirk Stolle

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
#include "../../../src/util/Strings.hpp"

TEST_CASE("endsWith")
{
  using namespace bvn;

  SECTION("empty strings")
  {
    REQUIRE_FALSE( endsWith("", ".png") );
    REQUIRE( endsWith("blah", "") );
  }

  SECTION("non-empty strings")
  {
    REQUIRE( endsWith("foo.png", ".png") );
    REQUIRE_FALSE( endsWith("foo.PNG", ".png") );
    REQUIRE( endsWith("foo.PNG", ".PNG") );
    REQUIRE_FALSE( endsWith("foo.PNG", "foo") );
  }

  SECTION("string is shorter than perspective suffix")
  {
     REQUIRE_FALSE( endsWith("abc", "abcdef") );
     REQUIRE_FALSE( endsWith("abc", "aaaaaaaaaa") );
  }
}

TEST_CASE("split strings")
{
  using namespace bvn;

  SECTION("empty string splitting returns empty string in vector")
  {
    const std::vector<std::string> vectorOfEmpty = { "" };
    REQUIRE( vectorOfEmpty == split("", ',') );
    REQUIRE( vectorOfEmpty == split("", ';') );
    REQUIRE( vectorOfEmpty == split("", '\t') );
    REQUIRE( vectorOfEmpty == split("", 'c') );
  }

  SECTION("normal string splitting")
  {
    REQUIRE( std::vector<std::string>({ "abc", "def", "ghi" }) == split("abc,def,ghi", ',') );
    REQUIRE( std::vector<std::string>({ "1", "2", "3" }) == split("1,2,3", ',') );
  }

  SECTION("splitting using different separator character")
  {
    REQUIRE( std::vector<std::string>({ "abc", "def", "ghi" }) == split("abc;def;ghi", ';') );
    REQUIRE( std::vector<std::string>({ "abc", "def", "ghi" }) == split("abc:def:ghi", ':') );
    REQUIRE( std::vector<std::string>({ "abc", "def", "ghi" }) == split("abc\tdef\tghi", '\t') );
    REQUIRE( std::vector<std::string>({ "1", "2", "3" }) == split("1?2?3", '?') );
    REQUIRE( std::vector<std::string>({ "1", "2", "3" }) == split("1 2 3", ' ') );
    REQUIRE( std::vector<std::string>({ "1", "2", "3" }) == split("1\n2\n3", '\n') );
  }

  SECTION("splitting using wrong separator does not split")
  {
    REQUIRE( std::vector<std::string>({ "abc;def;ghi" }) == split("abc;def;ghi", ',') );
    REQUIRE( std::vector<std::string>({ "abc;def;ghi" }) == split("abc;def;ghi", '.') );
    REQUIRE( std::vector<std::string>({ "abc;def;ghi" }) == split("abc;def;ghi", 'z') );
    REQUIRE( std::vector<std::string>({ "abc;def;ghi" }) == split("abc;def;ghi", ' ') );
  }

  SECTION("string splitting with empty parts preserves those parts")
  {
    REQUIRE( std::vector<std::string>({ "", "def", "ghi" }) == split(",def,ghi", ',') );
    REQUIRE( std::vector<std::string>({ "abc", "", "ghi" }) == split("abc,,ghi", ',') );
    REQUIRE( std::vector<std::string>({ "abc", "def", "" }) == split("abc,def,", ',') );
  }

  SECTION("splitting using NUL character as separator")
  {
    REQUIRE( std::vector<std::string>({ "i", "j", "k" }) == split(std::string("i\0j\0k", 5), '\0') );
  }
}

TEST_CASE("toLowerString")
{
  using namespace bvn;

  REQUIRE( toLowerString("") == "" );
  REQUIRE( toLowerString("abcdefghijklmnopqrstuvwxyz") == "abcdefghijklmnopqrstuvwxyz" );
  REQUIRE( toLowerString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == "abcdefghijklmnopqrstuvwxyz" );
  REQUIRE( toLowerString("AbCdEfGhIjKlMnOpQrStUvWxYz") == "abcdefghijklmnopqrstuvwxyz" );
  REQUIRE( toLowerString("This is a BIG test.") == "this is a big test." );
  REQUIRE( toLowerString(" !\"#$%&'()*+,-./") == " !\"#$%&'()*+,-./" );
  REQUIRE( toLowerString("0123456789") == "0123456789" );
  REQUIRE( toLowerString(":;<=>?@") == ":;<=>?@" );
  REQUIRE( toLowerString("[\\]^_`{|}~") == "[\\]^_`{|}~" );
}

TEST_CASE("trim")
{
  using namespace bvn;

  SECTION("empty string")
  {
    std::string s;
    trim(s);
    REQUIRE( s == "" );
  }

  SECTION("left side")
  {
    std::string s;

    s = "     abc";
    trim(s);
    REQUIRE( s == "abc" );

    s = "\t\t \t foo";
    trim(s);
    REQUIRE( s == "foo" );


    s = "\t\t \t ";
    trim(s);
    REQUIRE( s == "" );
  }

  SECTION("right side")
  {
    std::string s;

    s = "abc     ";
    trim(s);
    REQUIRE( s == "abc" );

    s = "foo\t\t \t ";
    trim(s);
    REQUIRE( s == "foo" );
  }

  SECTION("both sides")
  {
    std::string s;

    s = "  abc     ";
    trim(s);
    REQUIRE( s == "abc" );

    s = "  \t  \t  foo\t\t \t ";
    trim(s);
    REQUIRE( s == "foo" );
  }
}
