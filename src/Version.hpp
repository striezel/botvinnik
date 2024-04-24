/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022, 2023, 2024  Dirk Stolle

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

#ifndef BVN_VERSION_HPP
#define BVN_VERSION_HPP

#include <string>

namespace bvn
{

/** \brief version information */
const std::string version = "version 0.9.3, 2024-04-24";

/** \brief the User-Agent to use when sending requests to the Matrix server
 *
 * \remark User-Agent is disabled by default.
 */
const std::string userAgent = "botvinnik/0.9.3";

} // namespace

#endif // BVN_VERSION_HPP
