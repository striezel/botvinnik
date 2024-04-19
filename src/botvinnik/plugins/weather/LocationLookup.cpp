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

#include "LocationLookup.hpp"
#include "LocationLookupOpenMeteo.hpp"
#include "LocationLookupOpenStreetMap.hpp"

namespace bvn
{

nonstd::expected<Location, std::string> LocationLookup::find_location(const std::string_view location_name)
{
  auto location = LocationLookupOpenStreetMap::find_location(location_name);
  if (!location.has_value())
  {
    // Fall back to Open-Meteo geo-coding API.
    location = LocationLookupOpenMeteo::find_location(location_name);
  }

  return location;
}

} // namespace
