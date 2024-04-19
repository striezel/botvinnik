/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2017, 2020, 2023, 2024  Dirk Stolle

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

#ifndef BVN_STRINGS_HPP
#define BVN_STRINGS_HPP

#include <string>
#include <vector>

namespace bvn
{

/** \brief Splits a string into several strings, using the given separator.
 *
 * \param line    the string that shall be split
 * \param separator   character that works as separator for parts / substrings
 * \return Returns a vector of strings, containing the split strings.
 */
std::vector<std::string> split(std::string line, const char separator = ' ');

/** \brief Returns the lower case version of the given string.
 *
 * \param str  the string
 * \return Returns the string where all upper case characters are turned into
 *         lower case characters.
 */
std::string toLowerString(std::string str);


/** \brief Removes all leading and trailing spaces and (horizontal) tabulators
 *  from the given string.
 *
 * \param str1  the string that shall be trimmed
 */
void trim(std::string& str1);


/** \brief Tries to convert the string representation of an integer number into an int.
 *
 * \param str   the string that contains the number
 * \param value the int that will be used to store the result
 * \return Returns true on success, false on failure.
 * \remarks If false is returned, the value of parameter value is undefined.
 */
bool stringToInt(const std::string& str, int& value);


/** \brief Checks whether a string ends with a given suffix.
 *
 * \param str     the string to check
 * \param suffix  the suffix the string may end with
 * \return Returns true, if str ends with suffix.
 *         Returns false otherwise.
 */
bool endsWith(const std::string& str, const std::string& suffix);


/** \brief Gets the string representation of a floating-point value.
 *
 * \param d    the floating-point value to represent as string
 * \return Returns the string representation of the given double value.
 */
std::string doubleToString(const double d);

} // namespace

#endif // BVN_STRINGS_HPP
