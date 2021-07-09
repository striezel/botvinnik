/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#ifndef BVN_MATRIX_JSON_SYNC_HPP
#define BVN_MATRIX_JSON_SYNC_HPP

#include <string>
#include <vector>
#include "../../../third-party/simdjson/simdjson.h"
#include "../Room.hpp"

namespace bvn
{

/** \brief Encapsulates functionality to parse JSON of sync requests.
 */
struct Sync
{
  public:
    /** \brief Parses JSON data from the sync endpoint of Matrix.
     *
     * \param doc   parsed JSON document
     * \param rooms vector of rooms to populate with parsed data
     * \param invitedRoomIds  vector of rooms ids to populate with parsed data
     * \return Returns true, if the parsing was successful.
     *         Returns false otherwise.
     */
    static bool parse(const simdjson::dom::element& doc, std::vector<matrix::Room>& rooms, std::vector<std::string>& invitedRoomIds);
  private:
    static const int JsonError; /**< non-zero error code for JSON errors */

    /** \brief Parses JSON data of joined rooms from a sync request.
     *
     * \param join   JSON object "join" from the sync request
     * \param rooms vector of rooms to populate with parsed data
     * \return Returns a non-zero error code, if parsing failed.
     *         Returns zero, if parsing was successful.
     */
    static int parseJoinedRooms(const simdjson::dom::object& join, std::vector<matrix::Room>& rooms);


    /** \brief Parses JSON data of invited rooms from a sync request.
     *
     * \param invite   JSON object "invite" from the sync request
     * \param roomIds  vector of rooms ids to populate with parsed data
     * \return Returns a non-zero error code, if parsing failed.
     *         Returns zero, if parsing was successful.
     */
    static int parseInvitedRooms(const simdjson::dom::element& invite, std::vector<std::string>& roomIds);
}; // struct

} // namespace

#endif // BVN_MATRIX_JSON_SYNC_HPP
