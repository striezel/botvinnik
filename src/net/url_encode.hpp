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

#ifndef NET_URL_ENCODE_HPP
#define NET_URL_ENCODE_HPP

#include <string>
#include <string_view>

namespace bvn
{

/** \brief Encodes a string for use in URI / URL.
 *
 * \param str    the string to encode
 * \return Returns the encoded string.
 */
std::string urlencode(const std::string_view& str);

}

#endif // NET_URL_ENCODE_HPP
