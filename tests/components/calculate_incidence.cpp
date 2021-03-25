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
    for (int i = 1; i < 5; ++i)
    {
      CovidNumbersElem elem;
      elem.date = "2020-01-0" + std::to_string(i);
      elem.cases = i;
      elem.deaths = 0;
      elem.incidence7 = std::numeric_limits<double>::quiet_NaN();
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
      REQUIRE( std::isnan(incidence[idx].incidence7) );
    }
  }

  SECTION("calculate_incidence_7_elements")
  {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<CovidNumbersElem> numbers = {
      { 272, 11, nan, "2020-12-01" },
      { 400, 48, nan, "2020-12-02" },
      { 202, 19, nan, "2020-12-03" },
      { 119,  5, nan, "2020-12-04" },
      { 235, 18, nan, "2020-12-05" },
      { 234, 10, nan, "2020-12-06" },
      { 210, 26, nan, "2020-12-07" }
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
    for (std::size_t idx = 0; idx < 6; ++idx)
    {
      // Incidence values should not be set.
      REQUIRE( std::isnan(incidence[idx].incidence7) );
    }
    // Incidence for last entry should be set.
    REQUIRE_FALSE( std::isnan(incidence[6].incidence7) );
    // Incidence should be roughly 4.395170286.
    REQUIRE( incidence[6].incidence7 > 4.395170 );
    REQUIRE( incidence[6].incidence7 < 4.395171 );
  }

  SECTION("calculate_incidence_more_than_7_elements")
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
    for (std::size_t idx = 0; idx < 6; ++idx)
    {
      // Incidence values should not be set.
      REQUIRE( std::isnan(incidence[idx].incidence7) );
    }
    for (std::size_t idx = 6; idx < numbers.size(); ++idx)
    {
      // Incidence values should be set.
      REQUIRE_FALSE( std::isnan(incidence[idx].incidence7) );
    }
    // 6th:  143.81610676
    REQUIRE( incidence[6].incidence7 > 143.816106);
    REQUIRE( incidence[6].incidence7 < 143.816107);
    // 7th:  149.04381230
    REQUIRE( incidence[7].incidence7 > 149.043812);
    REQUIRE( incidence[7].incidence7 < 149.043813);
    // 8th:  151.26016672
    REQUIRE( incidence[8].incidence7 > 151.260166);
    REQUIRE( incidence[8].incidence7 < 151.260167);
    // 9th:  152.78511493
    REQUIRE( incidence[9].incidence7 > 152.785114);
    REQUIRE( incidence[9].incidence7 < 152.785115);
    // 10th: 152.76102412
    REQUIRE( incidence[10].incidence7 > 152.761024);
    REQUIRE( incidence[10].incidence7 < 152.761025);
    // 11th: 154.29440411
    REQUIRE( incidence[11].incidence7 > 154.294404);
    REQUIRE( incidence[11].incidence7 < 154.294405);
    // 12th: 156.55412199
    REQUIRE( incidence[12].incidence7 > 156.554121);
    REQUIRE( incidence[12].incidence7 < 156.554122);
    // 13th: 159.00656634
    REQUIRE( incidence[13].incidence7 > 159.006566);
    REQUIRE( incidence[13].incidence7 < 159.006567);
    // 14th: 157.87670740
    REQUIRE( incidence[14].incidence7 > 157.876707);
    REQUIRE( incidence[14].incidence7 < 157.876708);
    // 15th: 158.99693002
    REQUIRE( incidence[15].incidence7 > 158.996930);
    REQUIRE( incidence[15].incidence7 < 158.996931);
    // 16th: 155.93860182
    REQUIRE( incidence[16].incidence7 > 155.938601);
    REQUIRE( incidence[16].incidence7 < 155.938602);
    // 17th: 154.83885639
    REQUIRE( incidence[17].incidence7 > 154.838856);
    REQUIRE( incidence[17].incidence7 < 154.838857);
    // 18th: 153.72345194
    REQUIRE( incidence[18].incidence7 > 153.723451);
    REQUIRE( incidence[18].incidence7 < 153.723452);
    // 19th: 154.61842549
    REQUIRE( incidence[19].incidence7 > 154.618425);
    REQUIRE( incidence[19].incidence7 < 154.618426);
    // 20th: 154.74610678
    REQUIRE( incidence[20].incidence7 > 154.746106);
    REQUIRE( incidence[20].incidence7 < 154.746107);
    // 21st: 155.3519906
    REQUIRE( incidence[21].incidence7 > 155.351990);
    REQUIRE( incidence[21].incidence7 < 155.351991);
    // 22nd: 153.8993148
    REQUIRE( incidence[22].incidence7 > 153.899314);
    REQUIRE( incidence[22].incidence7 < 153.899315);
    // 23rd: 153.9474964
    REQUIRE( incidence[23].incidence7 > 153.947496);
    REQUIRE( incidence[23].incidence7 < 153.947497);
    // 24th: 152.9055689
    REQUIRE( incidence[24].incidence7 > 152.905568);
    REQUIRE( incidence[24].incidence7 < 152.905569);
    // 25th: 154.1968363
    REQUIRE( incidence[25].incidence7 > 154.196836);
    REQUIRE( incidence[25].incidence7 < 154.196837);
    // 26th: 153.7860880
    REQUIRE( incidence[26].incidence7 > 153.786088);
    REQUIRE( incidence[26].incidence7 < 153.786089);
    // 27th: 152.7718649
    REQUIRE( incidence[27].incidence7 > 152.771864);
    REQUIRE( incidence[27].incidence7 < 152.771865);
    // 28th: 151.2433031
    REQUIRE( incidence[28].incidence7 > 151.243303);
    REQUIRE( incidence[28].incidence7 < 151.243304);
    // 29th: 149.8821724
    REQUIRE( incidence[29].incidence7 > 149.882172);
    REQUIRE( incidence[29].incidence7 < 149.882173);
    // 30th: 150.2495572
    REQUIRE( incidence[30].incidence7 > 150.249557);
    REQUIRE( incidence[30].incidence7 < 150.249558);
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
        REQUIRE( std::isnan(incidence[idx].incidence7) );
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
        REQUIRE( std::isnan(incidence[idx].incidence7) );
      }
    }
  }
}
