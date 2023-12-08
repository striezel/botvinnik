/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2022, 2023  Dirk Stolle

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

#ifndef BVN_PLUGIN_PING_HPP
#define BVN_PLUGIN_PING_HPP

#include "DeactivatablePlugin.hpp"

namespace bvn
{

/** \brief Replies with the time it took to reach the bot.
 */
class Ping final: public DeactivatablePlugin
{
  public:
    /** \brief Constructor.
     *
     * \param bot_sync_delay   the configured synchronization delay of the bot
     */
    Ping(const std::chrono::milliseconds& bot_sync_delay);


    /** \brief Gets a list of commands that are provided by this plugin.
     *
     * \return Returns a vector of command names implemented by this plugin.
     */
    std::vector<std::string> commands() const override;


    /** \brief Reacts to a given command.
     *
     * \param command   name of the command to handle
     * \param message   complete text message that triggered the command
     * \param userId    id of the user that sent the message
     * \param roomId    id of the room that contains the message
     * \param server_ts timestamp when the message reached the server
     * \return Returns a message to send as reply to the command.
     *         If the return value is empty, no message will be sent.
     */
    Message handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts) override;


    /** \brief Gets a short, one line help text for a command.
     *
     * \param command  name of the command where help is requested
     * \return Returns a short, one line help text for the command.
     */
    std::string helpOneLine(const std::string_view& command) const override;


    /** \brief Gets a longer help text for a command.
     *
     * \param command  name of the command where help is requested
     * \param prefix   the prefix for bot commands, e. g. "!"
     * \return Returns a Message containing a longer help text for the command.
     */
    Message helpExtended(const std::string_view& command, const std::string_view& prefix) const override;
  private:
    std::chrono::milliseconds sync_delay; /**< bot's synchronization delay */
}; // class

} // namespace

#endif // BVN_PLUGIN_PING_HPP
