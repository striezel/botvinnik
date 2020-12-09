/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
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

#include "CovidNumbers.hpp"
#include <cmath>
#include <limits>
#include <sstream>

namespace bvn
{

CovidNumbersElem::CovidNumbersElem()
: cases(-1),
  deaths(-1),
  incidence14(std::numeric_limits<double>::quiet_NaN()),
  date(std::string())
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

Country::Country(const int64_t id, const std::string& _name, const std::string& _geoId)
: countryId(id),
  name(_name),
  geoId(_geoId)
{
}

} // namespace
