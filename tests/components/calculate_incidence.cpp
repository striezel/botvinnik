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
#include <cmath>
#include <limits>
#include "../../src/botvinnik/plugins/corona/CovidNumbers.hpp"

TEST_CASE("calculate_incidence")
{
  using namespace bvn;

  SECTION("calculate_incidence_few_elements")
  {
    std::vector<CovidNumbersElem> numbers;
    for (int i = 1; i < 10; ++i)
    {
      CovidNumbersElem elem;
      elem.date = "2020-01-0" + std::to_string(i);
      elem.cases = i;
      elem.deaths = 0;
      elem.incidence14 = std::numeric_limits<double>::quiet_NaN();
      numbers.push_back(elem);
    }
    const auto incidence = calculate_incidence(numbers, 123456789);
    REQUIRE( numbers.size() == incidence.size() );
    for (std::size_t idx = 0; idx < numbers.size(); ++idx)
    {
      // Numbers should be equal.
      REQUIRE( numbers[idx].date == incidence[idx].date );
      REQUIRE( numbers[idx].cases == incidence[idx].cases );
      REQUIRE( numbers[idx].deaths == incidence[idx].deaths );
      // Incidence values should not be set.
      REQUIRE( std::isnan(incidence[idx].incidence14) );
    }
  }

  SECTION("calculate_incidence_14_elements")
  {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<CovidNumbersElem> numbers = {
      { 272, 11, nan, "2020-12-01" },
      { 400, 48, nan, "2020-12-02" },
      { 202, 19, nan, "2020-12-03" },
      { 119,  5, nan, "2020-12-04" },
      { 235, 18, nan, "2020-12-05" },
      { 234, 10, nan, "2020-12-06" },
      { 210, 26, nan, "2020-12-07" },
      { 200,  6, nan, "2020-12-08" },
      { 135, 13, nan, "2020-12-09" },
      { 202, 16, nan, "2020-12-10" },
      {  63, 10, nan, "2020-12-11" },
      { 113, 11, nan, "2020-12-12" },
      { 298,  9, nan, "2020-12-13" },
      { 746,  6, nan, "2020-12-14" }
    };

    const auto incidence = calculate_incidence(numbers, 38041757);
    REQUIRE( numbers.size() == incidence.size() );
    for (std::size_t idx = 0; idx < numbers.size(); ++idx)
    {
      // Numbers should be equal.
      REQUIRE( numbers[idx].date == incidence[idx].date );
      REQUIRE( numbers[idx].cases == incidence[idx].cases );
      REQUIRE( numbers[idx].deaths == incidence[idx].deaths );
    }
    // Check incidence.
    for (std::size_t idx = 0; idx < 13; ++idx)
    {
      // Incidence values should not be set.
      REQUIRE( std::isnan(incidence[idx].incidence14) );
    }
    // Incidence for last entry should be set.
    REQUIRE_FALSE( std::isnan(incidence[13].incidence14) );
    // Incidence should be roughly 9.01377925.
    REQUIRE( incidence[13].incidence14 > 9.013779 );
    REQUIRE( incidence[13].incidence14 < 9.013780 );
  }

  SECTION("calculate_incidence_more_than_14_elements")
  {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<CovidNumbersElem> numbers = {
      { 19059, 103, nan, "2020-10-31" },
      { 14177,  29, nan, "2020-11-01" },
      { 12097,  49, nan, "2020-11-02" },
      { 15352, 131, nan, "2020-11-03" },
      { 17214, 151, nan, "2020-11-04" },
      { 19990, 118, nan, "2020-11-05" },
      { 21506, 166, nan, "2020-11-06" },
      { 23399, 130, nan, "2020-11-07" },
      { 16017,  63, nan, "2020-11-08" },
      { 13363,  63, nan, "2020-11-09" },
      { 15332, 154, nan, "2020-11-10" },
      { 18487, 261, nan, "2020-11-11" },
      { 21866, 215, nan, "2020-11-12" },
      { 23542, 218, nan, "2020-11-13" },
      { 22461, 178, nan, "2020-11-14" },
      { 16947, 107, nan, "2020-11-15" },
      { 10824,  62, nan, "2020-11-16" },
      { 14419, 267, nan, "2020-11-17" },
      { 17561, 305, nan, "2020-11-18" },
      { 22609, 251, nan, "2020-11-19" },
      { 23648, 260, nan, "2020-11-20" },
      { 22964, 254, nan, "2020-11-21" },
      { 15741, 138, nan, "2020-11-22" },
      { 10864,  90, nan, "2020-11-23" },
      { 13554, 249, nan, "2020-11-24" },
      { 18633, 410, nan, "2020-11-25" },
      { 22268, 389, nan, "2020-11-26" },
      { 22806, 426, nan, "2020-11-27" },
      { 21695, 379, nan, "2020-11-28" },
      { 14611, 158, nan, "2020-11-29" },
      { 11169, 125, nan, "2020-11-30" },
    };

    const auto incidence = calculate_incidence(numbers, 83019213);
    REQUIRE( numbers.size() == incidence.size() );
    for (std::size_t idx = 0; idx < numbers.size(); ++idx)
    {
      // Numbers should be equal.
      REQUIRE( numbers[idx].date == incidence[idx].date );
      REQUIRE( numbers[idx].cases == incidence[idx].cases );
      REQUIRE( numbers[idx].deaths == incidence[idx].deaths );
    }
    // Check incidence.
    for (std::size_t idx = 0; idx < 13; ++idx)
    {
      // Incidence values should not be set.
      REQUIRE( std::isnan(incidence[idx].incidence14) );
    }
    for (std::size_t idx = 13; idx < numbers.size(); ++idx)
    {
      // Incidence values should be set.
      REQUIRE_FALSE( std::isnan(incidence[idx].incidence14) );
    }
    // 13th: 302.82267311
    REQUIRE( incidence[13].incidence14 > 302.822673);
    REQUIRE( incidence[13].incidence14 < 302.822674);
    // 14th: 306.92051971
    REQUIRE( incidence[14].incidence14 > 306.920519);
    REQUIRE( incidence[14].incidence14 < 306.920520);
    // 15th: 310.25709675
    REQUIRE( incidence[15].incidence14 > 310.257096);
    REQUIRE( incidence[15].incidence14 < 310.257097);
    // 16th: 308.72371676
    REQUIRE( incidence[16].incidence14 > 308.723716);
    REQUIRE( incidence[16].incidence14 < 308.723717);
    // 17th: 307.59988052
    REQUIRE( incidence[17].incidence14 > 307.599880);
    REQUIRE( incidence[17].incidence14 < 307.599881);
    // 18th: 308.01785606
    REQUIRE( incidence[18].incidence14 > 308.017856);
    REQUIRE( incidence[18].incidence14 < 308.017857);
    // 19th: 311.17254749
    REQUIRE( incidence[19].incidence14 > 311.172547);
    REQUIRE( incidence[19].incidence14 < 311.172548);
    // 20th: 313.75267313
    REQUIRE( incidence[20].incidence14 > 313.752673);
    REQUIRE( incidence[20].incidence14 < 313.752674);
    // 21st: 313.22869804
    REQUIRE( incidence[21].incidence14 > 313.228698);
    REQUIRE( incidence[21].incidence14 < 313.228699);
    // 22nd: 312.89624487
    REQUIRE( incidence[22].incidence14 > 312.896244);
    REQUIRE( incidence[22].incidence14 < 312.896245);
    // 23rd: 309.88609829
    REQUIRE( incidence[23].incidence14 > 309.886098);
    REQUIRE( incidence[23].incidence14 < 309.886099);
    // 24th: 307.74442538
    REQUIRE( incidence[24].incidence14 > 307.744425);
    REQUIRE( incidence[24].incidence14 < 307.744426);
    // 25th: 307.92028828
    REQUIRE( incidence[25].incidence14 > 307.920288);
    REQUIRE( incidence[25].incidence14 < 307.920289);
    // 26th: 308.40451354
    REQUIRE( incidence[26].incidence14 > 308.404513);
    REQUIRE( incidence[26].incidence14 < 308.404514);
    // 27th: 307.51797177
    REQUIRE( incidence[27].incidence14 > 307.517971);
    REQUIRE( incidence[27].incidence14 < 307.517972);
    // 28th: 306.59529379
    REQUIRE( incidence[28].incidence14 > 306.595293);
    REQUIRE( incidence[28].incidence14 < 306.595294);
    // 29th: 303.7814873
    REQUIRE( incidence[29].incidence14 > 303.781487);
    REQUIRE( incidence[29].incidence14 < 303.781488);
    // 30th: 304.19705376
    REQUIRE( incidence[30].incidence14 > 304.197053);
    REQUIRE( incidence[30].incidence14 < 304.197054);
  }

  SECTION("calculate_incidence_no_population")
  {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<CovidNumbersElem> numbers = {
      { 19059, 103, nan, "2020-10-31" },
      { 14177,  29, nan, "2020-11-01" },
      { 12097,  49, nan, "2020-11-02" },
      { 15352, 131, nan, "2020-11-03" },
      { 17214, 151, nan, "2020-11-04" },
      { 19990, 118, nan, "2020-11-05" },
      { 21506, 166, nan, "2020-11-06" },
      { 23399, 130, nan, "2020-11-07" },
      { 16017,  63, nan, "2020-11-08" },
      { 13363,  63, nan, "2020-11-09" },
      { 15332, 154, nan, "2020-11-10" },
      { 18487, 261, nan, "2020-11-11" },
      { 21866, 215, nan, "2020-11-12" },
      { 23542, 218, nan, "2020-11-13" },
      { 22461, 178, nan, "2020-11-14" },
      { 16947, 107, nan, "2020-11-15" },
      { 10824,  62, nan, "2020-11-16" },
      { 14419, 267, nan, "2020-11-17" },
      { 17561, 305, nan, "2020-11-18" },
      { 22609, 251, nan, "2020-11-19" },
      { 23648, 260, nan, "2020-11-20" },
      { 22964, 254, nan, "2020-11-21" },
      { 15741, 138, nan, "2020-11-22" },
      { 10864,  90, nan, "2020-11-23" },
      { 13554, 249, nan, "2020-11-24" },
      { 18633, 410, nan, "2020-11-25" },
      { 22268, 389, nan, "2020-11-26" },
      { 22806, 426, nan, "2020-11-27" },
      { 21695, 379, nan, "2020-11-28" },
      { 14611, 158, nan, "2020-11-29" },
      { 11169, 125, nan, "2020-11-30" },
    };

    {
      const auto incidence = calculate_incidence(numbers, 0);
      REQUIRE( numbers.size() == incidence.size() );
      for (std::size_t idx = 0; idx < numbers.size(); ++idx)
      {
        // Numbers should be equal.
        REQUIRE( numbers[idx].date == incidence[idx].date );
        REQUIRE( numbers[idx].cases == incidence[idx].cases );
        REQUIRE( numbers[idx].deaths == incidence[idx].deaths );
        // Incidence values should not be set.
        REQUIRE( std::isnan(incidence[idx].incidence14) );
      }
    }

    // Same game for negative population.
    {
      const auto incidence = calculate_incidence(numbers, -1);
      REQUIRE( numbers.size() == incidence.size() );
      for (std::size_t idx = 0; idx < numbers.size(); ++idx)
      {
        // Numbers should be equal.
        REQUIRE( numbers[idx].date == incidence[idx].date );
        REQUIRE( numbers[idx].cases == incidence[idx].cases );
        REQUIRE( numbers[idx].deaths == incidence[idx].deaths );
        // Incidence values should not be set.
        REQUIRE( std::isnan(incidence[idx].incidence14) );
      }
    }
  }
}
