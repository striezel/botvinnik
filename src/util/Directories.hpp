/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2018, 2020  Dirk Stolle

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

#ifndef BVN_DIRECTORIES_HPP
#define BVN_DIRECTORIES_HPP

#include <string>

namespace bvn
{

namespace filesystem
{

#if defined(_WIN32)
  const char pathDelimiter = '\\';
#elif defined(__linux__) || defined(linux)
  const char pathDelimiter = '/';
#else
  #error Unknown operating system!
#endif

/** \brief Tries to determine the absolute path to the current user's home directory
 * and returns that path in the reference parameter in case of success. If the
 * directory's path cannot be determined, the function returns false and the
 * result string is unchanged.
 *
 * \param result the string that will hold the result, if successful
 * \return Returns true, if the home directory could be retrieved.
 *         Returns false otherwise.
 */
bool getHome(std::string& result);

} // namespace

} // namespace

#endif // BVN_DIRECTORIES_HPP
