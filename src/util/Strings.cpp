/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2017, 2020, 2023  Dirk Stolle

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

#include "Strings.hpp"
#include <limits>
#include <sstream>

namespace bvn
{

std::vector<std::string> split(std::string line, const char separator)
{
  std::vector<std::string> result;
  std::string::size_type pos = line.find(separator);
  while (pos != std::string::npos)
  {
    result.push_back(line.substr(0, pos));
    line.erase(0, pos + 1);
    pos = line.find(separator);
  }
  result.push_back(line);
  return result;
}

std::string toLowerString(std::string str)
{
  // Assume ASCII or compatible charset, where distance of the codepoint of
  // the uppercase and lowercase version of a letter is always the same.
  constexpr int diff = 'a' - 'A';
  for (unsigned int i = 0; i < str.size(); ++i)
  {
    if ((str[i] >= 'A') && (str[i] <= 'Z'))
    {
      str[i] = str[i] + diff;
    }
  } // for
  return str;
}

void trimLeft(std::string& str1)
{
  if (str1.empty())
  {
    return;
  }
  // trim stuff at begin
  const int len = str1.length();
  int pos = 0;
  bool go_on = true;
  while (go_on)
  {
    const char ch = str1.at(pos);
    if ((ch == ' ') || (ch == '\t'))
    {
      ++pos;
      go_on = (pos<len);
    }
    else
    {
      go_on = false;
    }
  } // while
  if (pos >= len)
  {
    str1.clear();
    return;
  }
  else if (pos > 0)
  {
    str1.erase(0, pos);
  }
  return;
}

void trimRight(std::string& str1)
{
  if (str1.empty())
  {
    return;
  }
  // trim stuff at end
  const int len = str1.length();
  int pos = len - 1;
  bool go_on = true;
  while (go_on)
  {
    const char ch = str1.at(pos);
    if ((ch == ' ') || (ch == '\t'))
    {
      --pos;
      go_on = (pos >= 0);
    }
    else
    {
      go_on = false;
    }
  } // while
  if (pos == -1)
  {
    str1.clear();
  }
  else if (pos < len - 1)
  {
    str1.erase(pos + 1);
  }
  return;
}

void trim(std::string& str1)
{
  trimLeft(str1);
  trimRight(str1);
  return;
}

bool stringToInt(const std::string& str, int& value)
{
  if (str.empty())
  {
    return false;
  }
  value = 0;
  unsigned int i;
  constexpr int cTenthLimit = std::numeric_limits<int>::max() / 10;
  constexpr int cRealLimit = std::numeric_limits<int>::max();
  bool negative;
  if (str[0] == '-')
  {
    i = 1;
    negative = true;
  }
  else
  {
    i = 0;
    negative = false;
  }
  for ( ; i < str.size(); ++i)
  {
    if ((str[i] >= '0') && (str[i] <= '9'))
    {
      /* If the result of the multiplication in the next line would go out of
         the type range, then the result is not useful anyway, so quit here. */
      if (value > cTenthLimit)
      {
        return false;
      }
      value = value * 10;
      /* If the result of the addition in the next line would go out of the
         type's range, then the result is not useful anyway, so quit here. */
      if (value > cRealLimit - (str[i] - '0'))
      {
        return false;
      }
      value = value + (str[i] - '0');
    } // if
    else
    {
      // unknown or invalid character detected
      return false;
    }
  } // for
  if (negative)
  {
    value = -value;
  }
  return true;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
  const std::string::size_type strLen = str.size();
  const std::string::size_type suffixLen = suffix.size();
  if (strLen < suffixLen)
  {
    return false;
  }
  return (str.substr(strLen - suffixLen).compare(suffix) == 0);
}

std::string doubleToString(const double d)
{
  std::stringstream s_str;
  s_str << d;
  return s_str.str();
}

} // namespace
