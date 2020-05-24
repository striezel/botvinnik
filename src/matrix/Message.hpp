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

#ifndef BVN_MESSAGE_HPP
#define BVN_MESSAGE_HPP

#include <string>

namespace bvn
{

/** \brief Represents a text message.
 */
struct Message
{
  public:
    /** \brief Constructs an empty message.
     */
    Message();


    /** \brief Constructs a message with predefined text.
     *
     * \param plain   plain text (i. e. unformatted)
     * \param formatted  formatted version of the text (i. e. HTML)
     */
    explicit Message(const std::string& plain, const std::string& formatted = "");


    std::string body;   /**< textual representation of the message */
    std::string formatted_body; /**< formatted body, e. g. HTML (optional) */
}; // struct

} // namespace

#endif // BVN_MESSAGE_HPP
