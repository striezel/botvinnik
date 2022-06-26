/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2021  Dirk Stolle

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
#include "../../src/net/htmlspecialchars.hpp"

TEST_CASE("htmlspecialchars")
{
  using namespace bvn;

  SECTION("no replacements")
  {
    REQUIRE( htmlspecialchars("") == "" );
    REQUIRE( htmlspecialchars(" ") == " " );
    REQUIRE( htmlspecialchars("\t    ") == "\t    " );
    REQUIRE( htmlspecialchars("abc\ndef") == "abc\ndef" );
    REQUIRE( htmlspecialchars("abc\r\ndef") == "abc\r\ndef" );
  }

  SECTION("simple replacements")
  {
    REQUIRE( htmlspecialchars("<a href=\"example.com/\">example</a>")
             == "&lt;a href=&quot;example.com/&quot;&gt;example&lt;/a&gt;" );
    REQUIRE( htmlspecialchars("&") == "&amp;" );
    REQUIRE( htmlspecialchars("&amp;") == "&amp;amp;" );
    REQUIRE( htmlspecialchars("<") == "&lt;" );
    REQUIRE( htmlspecialchars("&lt;") == "&amp;lt;" );
    REQUIRE( htmlspecialchars(">") == "&gt;" );
    REQUIRE( htmlspecialchars("&gt;") == "&amp;gt;" );
    REQUIRE( htmlspecialchars("\"") == "&quot;" );
    REQUIRE( htmlspecialchars("&quot;") == "&amp;quot;" );
  }

  SECTION("multiple replacements")
  {
    REQUIRE( htmlspecialchars("&&&&&") == "&amp;&amp;&amp;&amp;&amp;" );
    REQUIRE( htmlspecialchars("<<<<<") == "&lt;&lt;&lt;&lt;&lt;" );
    REQUIRE( htmlspecialchars(">>>>>") == "&gt;&gt;&gt;&gt;&gt;" );
    REQUIRE( htmlspecialchars("&gt;") == "&amp;gt;" );
    REQUIRE( htmlspecialchars("\"\"\"\"\"") == "&quot;&quot;&quot;&quot;&quot;" );
    REQUIRE( htmlspecialchars("The characters <, >, &, and \" get replaced.") == "The characters &lt;, &gt;, &amp;, and &quot; get replaced." );
  }
}
