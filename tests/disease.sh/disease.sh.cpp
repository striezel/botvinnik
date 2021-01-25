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

#include <catch.hpp>
#include "../../src/botvinnik/plugins/corona/disease.sh.hpp"

TEST_CASE("disease.sh API")
{
  using namespace bvn::disease_sh;

  SECTION("historical_api")
  {
    const auto numbers = requestHistoricalApi("ES", false);
    REQUIRE( numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < numbers.days.size(); ++idx)
    {
      REQUIRE( numbers.days[idx-1].date < numbers.days[idx].date );
    }
    REQUIRE( numbers.totalCases > 0 );
    REQUIRE( numbers.totalDeaths > 0 );

    const auto all_numbers = requestHistoricalApi("ES", true);
    REQUIRE( all_numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < all_numbers.days.size(); ++idx)
    {
      REQUIRE( all_numbers.days[idx-1].date < all_numbers.days[idx].date );
    }
    REQUIRE( all_numbers.totalCases > 0 );
    REQUIRE( all_numbers.totalDeaths > 0 );

    // All data should have more entries than recent data.
    REQUIRE( all_numbers.days.size() > numbers.days.size());
  }

  SECTION("historical_api_province")
  {
    const auto numbers = requestHistoricalApiProvince("DK", "mainland", false);
    REQUIRE( numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < numbers.days.size(); ++idx)
    {
      REQUIRE( numbers.days[idx-1].date < numbers.days[idx].date );
    }
    REQUIRE( numbers.totalCases > 0 );
    REQUIRE( numbers.totalDeaths > 0 );

    const auto all_numbers = requestHistoricalApiProvince("DK", "mainland", true);
    REQUIRE( all_numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < all_numbers.days.size(); ++idx)
    {
      REQUIRE( all_numbers.days[idx-1].date < all_numbers.days[idx].date );
    }
    REQUIRE( all_numbers.totalCases > 0 );
    REQUIRE( all_numbers.totalDeaths > 0 );

    // All data should have more entries than recent data.
    REQUIRE( all_numbers.days.size() > numbers.days.size());
  }

  SECTION("historical_api_usa_counties")
  {
    const auto numbers = requestHistoricalApiUsaCounties("guam", false);
    REQUIRE( numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < numbers.days.size(); ++idx)
    {
      REQUIRE( numbers.days[idx-1].date < numbers.days[idx].date );
    }
    REQUIRE( numbers.totalCases > 0 );
    REQUIRE( numbers.totalDeaths > 0 );

    const auto all_numbers = requestHistoricalApiUsaCounties("guam", true);
    REQUIRE( all_numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < all_numbers.days.size(); ++idx)
    {
      REQUIRE( all_numbers.days[idx-1].date < all_numbers.days[idx].date );
    }
    REQUIRE( all_numbers.totalCases > 0 );
    REQUIRE( all_numbers.totalDeaths > 0 );

    // All data should have more entries than recent data.
    REQUIRE( all_numbers.days.size() > numbers.days.size());
  }

  SECTION("requestHistoricalApiFirstOfMultipleProvinces")
  {
    const auto numbers = requestHistoricalApiFirstOfMultipleProvinces("NL", "bonaire%2C%20sint%20eustatius%20and%20saba%7C", false);
    REQUIRE( numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < numbers.days.size(); ++idx)
    {
      REQUIRE( numbers.days[idx-1].date < numbers.days[idx].date );
    }
    REQUIRE( numbers.totalCases > 0 );
    REQUIRE( numbers.totalDeaths > 0 );

    const auto all_numbers = requestHistoricalApiFirstOfMultipleProvinces("NL", "bonaire%2C%20sint%20eustatius%20and%20saba%7C", true);
    REQUIRE( all_numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < all_numbers.days.size(); ++idx)
    {
      REQUIRE( all_numbers.days[idx-1].date < all_numbers.days[idx].date );
    }
    REQUIRE( all_numbers.totalCases > 0 );
    REQUIRE( all_numbers.totalDeaths > 0 );

    // All data should have more entries than recent data.
    REQUIRE( all_numbers.days.size() > numbers.days.size());
  }

  SECTION("historical API: worldwide data")
  {
    const auto numbers = requestHistoricalApi("all", false);
    REQUIRE( numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < numbers.days.size(); ++idx)
    {
      REQUIRE( numbers.days[idx-1].date < numbers.days[idx].date );
    }
    REQUIRE( numbers.totalCases > 0 );
    REQUIRE( numbers.totalDeaths > 0 );

    const auto all_numbers = requestHistoricalApi("all", true);
    REQUIRE( all_numbers.days.size() > 0 );
    for (std::size_t idx = 1; idx < all_numbers.days.size(); ++idx)
    {
      REQUIRE( all_numbers.days[idx-1].date < all_numbers.days[idx].date );
    }
    REQUIRE( all_numbers.totalCases > 0 );
    REQUIRE( all_numbers.totalDeaths > 0 );

    // All data should have more entries than recent data.
    REQUIRE( all_numbers.days.size() > numbers.days.size());
  }
}
