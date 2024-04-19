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
#include <cmath>
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

std::string wmo_code_to_icon(const int code)
{
  // Comments on the entries state the following three things, in that order:
  // name of the weather condition - name of icon - Unicode code point.
  static const std::unordered_map<int, std::string> codes = {
      // clear sky - "Black Sun with Rays" - U+2600
      {  0, "\xE2\x98\x80" },
      // mainly clear - "White Sun with Small Cloud" - U+1F324
      {  1, "\xF0\x9F\x8C\xA4" },
      // partly cloudy - "White Sun behind cloud" - U+1F325
      {  2, "\xF0\x9F\x8C\xA5" },
      // overcast - "Cloud" - U+2601
      {  3, "\xE2\x98\x81" },
      // fog - "fog" - U+1F32B
      { 45, "\xF0\x9F\x8C\xAB" },
      // Depositing rime fog - "fog" - U+1F32B
      { 48, "\xF0\x9F\x8C\xAB" },
      // light drizzle - "Cloud with rain" - U+1F327
      { 51, "\xF0\x9F\x8C\xA7" },
      // Moderate drizzle - "Cloud with rain" - U+1F327
      { 53, "\xF0\x9F\x8C\xA7" },
      // Dense drizzle - "Cloud with rain" - U+1F327
      { 55, "\xF0\x9F\x8C\xA7" },
      // light freezing drizzle - "Cloud with rain" - U+1F327
      { 56, "\xF0\x9F\x8C\xA7" },
      // dense freezing drizzle - "Cloud with rain" - U+1F327
      { 57, "\xF0\x9F\x8C\xA7" },
      // Slight rain - "White Sun Behind Cloud with Rain" - U+1F326
      { 61, "\xF0\x9F\x8C\xA6" },
      // Moderate rain - "White Sun Behind Cloud with Rain" - U+1F326
      { 63, "\xF0\x9F\x8C\xA6" },
      // Dense rain - "White Sun Behind Cloud with Rain" - U+1F326
      { 65, "\xF0\x9F\x8C\xA6" },
      // Light freezing rain - "White Sun Behind Cloud with Rain" - U+1F326
      { 66, "\xF0\x9F\x8C\xA6" },
      // Heavy freezing rain - "White Sun Behind Cloud with Rain" - U+1F326
      { 67, "\xF0\x9F\x8C\xA6" },
      // Slight snow fall - "Snowflake" - U+2744
      { 71, "\xE2\x9D\x84" },
      // Moderate snow fall - "Snowflake" - U+2744
      { 73, "\xE2\x9D\x84" },
      // Heavy snow fall - "Snowflake" - U+2744
      { 75, "\xE2\x9D\x84" },
      // Snow grains - "Snowflake" - U+2744
      { 77, "\xE2\x9D\x84" },
      // Slight rain showers - "Cloud with rain" - U+1F327
      { 80, "\xF0\x9F\x8C\xA7" },
      // Moderate rain showers - "Cloud with rain" - U+1F327
      { 81, "\xF0\x9F\x8C\xA7" },
      // Heavy rain showers - "Cloud with rain" - U+1F327
      { 82, "\xF0\x9F\x8C\xA7" },
      // Slight snow showers - "Cloud with snow" - U+1F328
      { 85, "\xF0\x9F\x8C\xA8" },
      // Heavy snow showers - "Cloud with snow" - U+1F328
      { 86, "\xF0\x9F\x8C\xA8" },
      // Thunderstorm - "Cloud with Lightning" - U+1F329
      { 95, "\xF0\x9F\x8C\xA9" },
      // Thunderstorm with slight hail - "Cloud with Lightning" - U+1F329
      { 96, "\xF0\x9F\x8C\xA9" },
      // Thunderstorm with heavy hail - "Cloud with Lightning" - U+1F329
      { 99, "\xF0\x9F\x8C\xA9" },
  };
  const auto iter = codes.find(code);
  if (iter == codes.end())
  {
    return "";
  }
  return iter->second;
}

std::string wind_direction_to_text(const double direction)
{
  if ((direction > 360.0) || (direction < 0.0) || std::isnan(direction))
  {
    return "";
  }

  // 0° = N, 90° = E, 180° = S, 270° = W
  if ((direction < 22.5) || (direction >= 337.5))
    return "N";
  if (direction < 67.5)
    return "NE";
  if (direction < 112.5)
    return "E";
  if (direction < 157.5)
    return "SE";
  if (direction < 202.5)
    return "S";
  if (direction < 247.5)
    return "SW";
  if (direction < 292.5)
    return "W";
  else // direction < 337.5
    return "NW";
}

std::string wind_direction_to_arrow(const double direction)
{
  if ((direction > 360.0) || (direction < 0.0) || std::isnan(direction))
  {
    return "";
  }

  // 0° = N, 90° = E, 180° = S, 270° = W
  if ((direction < 22.5) || (direction >= 337.5))
    return "\xE2\x86\x93"; // N
  if (direction < 67.5)
    return "\xE2\x86\x99"; // NE
  if (direction < 112.5)
    return "\xE2\x86\x90"; // E
  if (direction < 157.5)
    return "\xE2\x86\x96"; // SE
  if (direction < 202.5)
    return "\xE2\x86\x91"; // S
  if (direction < 247.5)
    return "\xE2\x86\x97"; // SW
  if (direction < 292.5)
    return "\xE2\x86\x92"; // W
  else // direction < 337.5
    return "\xE2\x86\x98"; // NW
}

} // namespace
