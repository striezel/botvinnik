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

#ifndef BVN_PLUGIN_XKCDDB_HPP
#define BVN_PLUGIN_XKCDDB_HPP

#include <optional>
#include <string>
#include "../../../util/sqlite3.hpp"

namespace bvn::XkcdDb
{

/** \brief Gets the database containing the comic information.
 *
 * \return Returns an optional containing the database.
 *         Returns an empty optional, if database does not exist and cannot be created.
 */
std::optional<sql::database> getDatabase();


/** \brief Creates the database structure required to store information.
 *
 * \param db   open database connection
 * \return Returns true, if structure was created.
 *         Returns false, if an error occurred.
 */
bool createDbStructure(sql::database& db);


/** \brief  Gets the Matrix Content URI of a comic from the database.
 *
 * \param db   open database connection
 * \param num   number of the comic
 * \return Returns an optional containing the URI, if it was found.
 *         Returns an empty optional, if the comic is not in the database
 *         or an error occurred.
 */
std::optional<std::string> getMxcUri(sql::database& db, const unsigned int num);


/** \brief  Inserts the Matrix Content URI of a comic into the database.
 *
 * \param db       open database connection
 * \param num      number of the comic
 * \param mxcUri   Matrix content URI
 * \return Returns true, if insertion was successful.
 *         Returns false, if an error occurred.
 */
bool insertMxcUri(sql::database& db, const unsigned int num, const std::string& mxcUri);

} // namespace

#endif // BVN_PLUGIN_XKCDDB_HPP
