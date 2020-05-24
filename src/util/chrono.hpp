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

#ifndef BVN_CHRONO_HPP
#define BVN_CHRONO_HPP

#include <chrono>
#include <string>

namespace bvn
{

/** \brief Converts a time point to a human-readable date representation.
 *
 * \param dateTime   the time point
 * \return Returns a string representing the time point. It is similar to SQL
 *         dates, e. g. "2020-05-25 13:37:00" could be a return value.
 *         If conversion fails, it just returns the milliseconds since the
 *         epoch.
 */
std::string timePointToString(const std::chrono::time_point<std::chrono::system_clock>& dateTime);

/** \brief Converts current system time to a human-readable date representation.
 *
 * \return Returns a string representing the current time. It is similar to SQL
 *         dates, e. g. "2020-05-25 13:37:00" could be a return value.
 *         If conversion fails, it just returns the milliseconds since the
 *         epoch.
 */
std::string nowToString();

} // namespace

#endif // BVN_CHRONO_HPP
