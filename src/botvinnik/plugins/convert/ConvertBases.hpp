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

#ifndef BVN_CONVERTBASES_HPP
#define BVN_CONVERTBASES_HPP

#include <algorithm>
#include <string>
#include <type_traits>

namespace bvn
{

const std::string digits = "0123456789abcdef";

template<typename intT, typename baseT>
class ConvertBases
{
  static_assert(std::is_integral<intT>::value, "Template type intT in ConvertBases must be an integral type!");
  static_assert(std::is_unsigned<intT>::value, "Template type intT in ConvertBases shall be an unsigned type!");
  static_assert(std::is_integral<baseT>::value, "Template type baseT in ConvertBases must be an integral type!");
  static_assert(std::is_unsigned<baseT>::value, "Template type baseT in ConvertBases shall be an unsigned type!");

  private:

  public:
    static std::string get(intT n, const baseT base = 10);
}; // class

template<typename intT, typename baseT>
std::string ConvertBases<intT, baseT>::get(intT n, const baseT base)
{
  if (base < 2 || base > 16)
    return "Base must be in range [2;16]!";

  if (n < base)
    return std::string(1, digits.at(n));

  std::string result;

  while (n > 0)
  {
    result.push_back(digits[n % base]);
    n /= base;
  }
  // Reverse container, because order of digits comes out in reverse from loop
  // above.
  std::reverse(result.begin(), result.end());

  return result;
}

} // namespace

#endif // BVN_CONVERTBASES_HPP
