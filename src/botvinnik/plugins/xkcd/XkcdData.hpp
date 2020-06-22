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

#ifndef BVN_PLUGIN_XKCDDATA_HPP
#define BVN_PLUGIN_XKCDDATA_HPP

#include <optional>
#include <string>

namespace bvn
{

/** \brief Basic data for xkcd.com comic.
 */
struct XkcdData
{
  /** Default constructor. Sets everything to empty or zero. */
  XkcdData();

  /** \brief Gets data for a given comic.
   *
   * \param num   number of the comic
   * \return Returns data about the comic in case of success.
   *         Returns empty optional, if data retrieval failed.
   */
  static std::optional<XkcdData> get(unsigned int num);

  unsigned int num;  /**< number of comic */
  std::string title; /**< comic title */
  std::string img;   /**< URL for the image */
  std::string transcript; /**< transcribed text of comic (optional) */
  std::string alt;  /**< alt text of image */
}; // struct

} // namespace

#endif // BVN_PLUGIN_XKCDDATA_HPP
