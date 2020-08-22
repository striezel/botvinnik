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

#ifndef BVN_EVENT_POWERLEVELS_HPP
#define BVN_EVENT_POWERLEVELS_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace bvn::matrix
{

/** \brief Represents power levels of users in a Matrix room.
 *
 * See <https://matrix.org/docs/spec/client_server/r0.6.1#m-room-power-levels>
 * for a complete documentation. This struct contains only selected members
 * that are required by the bot.
 *
 * Example JSON:

    {
        "ban": 50,
        "events": {
            "m.room.avatar": 50,
            "m.room.canonical_alias": 50,
            "m.room.history_visibility": 100,
            "m.room.name": 50,
            "m.room.power_levels": 100
        },
        "events_default": 0,
        "invite": 0,
        "kick": 50,
        "redact": 50,
        "state_default": 50,
        "users": {
            "@alice:matrix.homeserver.tld": 100,
            "@bob:matrix.homeserver.tld": 100
        },
        "users_default": 0
    }

 */
struct PowerLevels
{
  /** \brief Constructs a PowerLevels struct with all members set to defaults.
   */
  PowerLevels();

  /** \brief type alias used for power level values
   */
  typedef unsigned short int level_t;

  level_t ban; /**< level required to ban a user; defaults to 50, if unspecified */
  level_t kick; /**< level required to kick a user; defaults to 50, if unspecified */
  level_t users_default; /**< default power level for every user - unless the user_id is specified in users. Defaults to 0, if unspecified. */
  std::unordered_map<std::string, level_t> users; /**< power levels for user ids */


  /** \brief Checks whether a given user has the power to ban or kick users.
   *
   * \param userId    id of the user to check
   * \return Returns true, if the user can ban or kick users.
   *         Returns false otherwise.
   */
  bool canBanOrKick(const std::string_view& userId) const;
}; // struct

} // namespace

#endif // BVN_EVENT_POWERLEVELS_HPP
