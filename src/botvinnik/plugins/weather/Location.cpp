/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
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

#include "Location.hpp"
#include <cmath>
#include <limits>

namespace bvn
{

Location::Location()
: latitude(std::numeric_limits<double>::quiet_NaN()),
  longitude(std::numeric_limits<double>::quiet_NaN()),
  name(""),
  display_name("")
{
}

bool Location::has_data() const
{
  return !std::isnan(latitude) && !std::isnan(longitude)
      && !name.empty() && !display_name.empty();
}

} // namespace
