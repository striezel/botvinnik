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

#include "FreeFunctions.hpp"
#include <unordered_map>

namespace bvn::weather
{

std::string wmo_code_to_text(const int code)
{
  static const std::unordered_map<int, std::string> codes = {
      {  0, "Clear sky" },
      {  1, "Mainly clear" },
      {  2, "Partly cloudy" },
      {  3, "Overcast" },
      { 45, "Fog" },
      { 48, "Depositing rime fog" },
      { 51, "Light drizzle" },
      { 53, "Moderate drizzle" },
      { 55, "Dense drizzle" },
      { 56, "Light freezing drizzle" },
      { 57, "Dense freezing drizzle" },
      { 61, "Slight rain" },
      { 63, "Moderate rain" },
      { 65, "Dense rain" },
      { 66, "Light freezing rain" },
      { 67, "Heavy freezing rain" },
      { 71, "Slight snow fall" },
      { 73, "Moderate snow fall" },
      { 75, "Heavy snow fall" },
      { 77, "Snow grains" },
      { 80, "Slight rain showers" },
      { 81, "Moderate rain showers" },
      { 82, "Heavy rain showers" },
      { 85, "Slight snow showers" },
      { 86, "Heavy snow showers" },
      { 95, "Thunderstorm" },
      { 96, "Thunderstorm with slight hail" },
      { 99, "Thunderstorm with heavy hail" },
  };
  const auto iter = codes.find(code);
  if (iter == codes.end())
  {
    return "Unknown weather condition (code " + std::to_string(code) + ")";
  }
  return iter->second;
}

} // namespace
