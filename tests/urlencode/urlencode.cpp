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

#include "../locate_catch.hpp"
#include <unordered_map>
#include "../../src/net/url_encode.hpp"

TEST_CASE("urlencoding")
{
  using namespace bvn;

  const std::unordered_map<std::string, std::string> cases = {
      { "", ""},
      { "foo", "foo"},
      { "FoObAr", "FoObAr"},
      { "@alice:example.tld", "%40alice%3Aexample.tld"},
      { "!room:example.tld", "%21room%3Aexample.tld"},
      { "some space", "some%20space"},
      { "Crème brûlée", "Cr%C3%A8me%20br%C3%BBl%C3%A9e" }
  };

  SECTION("escaped string must match expectation")
  {
    for (const auto& item : cases)
    {
      const auto escaped = urlencode(item.first);
      REQUIRE( escaped == item.second );
    }
  }
}
