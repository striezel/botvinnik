/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#include "CovidNumbers.hpp"
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>

namespace bvn
{

CovidNumbersElem::CovidNumbersElem()
: cases(-1),
  deaths(-1),
  incidence7(std::numeric_limits<double>::quiet_NaN()),
  date(std::string())
{
}

CovidNumbersElem::CovidNumbersElem(int64_t _cases, int64_t _deaths, double _incidence7, const std::string& _date)
: cases(_cases),
  deaths(_deaths),
  incidence7(_incidence7),
  date(_date)
{
}

CovidNumbers::CovidNumbers()
: totalCases(-1),
  totalDeaths(-1),
  days(std::vector<CovidNumbersElem>())
{
}

std::string CovidNumbers::percentage() const
{
  if (totalCases <= 0 || totalDeaths < 0)
    return std::string();

  const double p = std::round(static_cast<double>(totalDeaths) * 10000.0 / static_cast<double>(totalCases)) / 100.0;
  // String streams give nicer output format than std::to_string().
  std::ostringstream stream;
  stream << p << " %";
  return stream.str();
}

Country::Country(const int64_t id, const std::string& _name, const std::string& _geoId, const int64_t pop)
: countryId(id),
  name(_name),
  geoId(_geoId),
  population(pop)
{
}

std::vector<CovidNumbersElem> calculate_incidence(const std::vector<CovidNumbersElem>& numbers, const int32_t population)
{
  const auto len = numbers.size();
  std::vector<CovidNumbersElem> result;
  result.reserve(len);
  const int max_len = std::min(static_cast<std::vector<CovidNumbersElem>::size_type>(6), len);
  for (int i = 0; i < max_len; ++i)
  {
    CovidNumbersElem elem = numbers[i];
    elem.incidence7 = std::numeric_limits<double>::quiet_NaN();
    result.push_back(elem);
  }
  // If there is not enough data to ever get to 7 days, then there can be no
  // 7-day incidence.
  if (len <= 6)
  {
    return result;
  }
  // If there is no valid population number, no incidence can be calculated.
  if (population <= 0)
  {
    for (std::size_t i = 6; i < len; ++i)
    {
      result.push_back(numbers[i]);
      result.back().incidence7 = std::numeric_limits<double>::quiet_NaN();
    }
    return result;
  }

  auto accumulator = [](int32_t acc, const CovidNumbersElem& b) {
                         return acc + b.cases;
                     };
  int32_t sum = std::accumulate(numbers.begin(), numbers.begin() + 7, 0, accumulator);
  CovidNumbersElem elem = numbers[6];
  elem.incidence7 = static_cast<double>(sum) * 100000.0 / static_cast<double>(population);
  result.push_back(elem);
  for (std::size_t idx = 7; idx < len; ++idx)
  {
    // Recalculate sum.
    sum = sum + numbers[idx].cases - numbers[idx - 7].cases;
    elem = numbers[idx];
    elem.incidence7 = static_cast<double>(sum) * 100000.0 / static_cast<double>(population);
    result.push_back(elem);
  }

  return result;
}

} // namespace
