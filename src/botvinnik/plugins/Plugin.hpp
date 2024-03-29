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

#ifndef BVN_PLUGIN_HPP
#define BVN_PLUGIN_HPP

#include <chrono>
#include <string>
#include <string_view>
#include <vector>
#include "../../matrix/Message.hpp"

namespace bvn
{

/** \brief Basic interface for plugins.
 */
class Plugin
{
  public:
    /** \brief Virtual destructor to ensure derived destructors are called.
     */
    virtual ~Plugin() {}

    /** \brief Gets a list of commands that are provided by this plugin.
     *
     * \return Returns a vector of command names implemented by this plugin.
     */
    virtual std::vector<std::string> commands() const = 0;


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
    virtual Message handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts) = 0;


    /** \brief Gets a short, one line help text for a command.
     *
     * \param command  name of the command where help is requested
     * \return Returns a short, one line help text for the command.
     */
    virtual std::string helpOneLine(const std::string_view& command) const = 0;


    /** \brief Gets a longer help text for a command.
     *
     * \param command  name of the command where help is requested
     * \param prefix   the prefix for bot commands, e. g. "!"
     * \return Returns a Message containing a longer help text for the command.
     */
    virtual Message helpExtended(const std::string_view& command, const std::string_view& prefix) const = 0;


    /** \brief Determines whether a command of the plugin can be deactivated.
     *
     * \param command  name of the command to deactivate
     * \return Returns true, if the command can be deactivated.
     *         Returns false, if deactivation is forbidden.
     */
    virtual bool allowDeactivation(const std::string_view& command) const = 0;
}; // class

} // namespace

#endif // BVN_PLUGIN_HPP
