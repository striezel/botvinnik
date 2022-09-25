/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2022  Dirk Stolle

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

#ifndef BVN_PLUGIN_BASIC_HPP
#define BVN_PLUGIN_BASIC_HPP

#include <string>
#include <string_view>
#include <vector>
#include "../Plugin.hpp"
#include "../../Bot.hpp"

namespace bvn
{

/** \brief Plugin providing some basic commands for the bot.
 */
class Basic: public Plugin
{
  public:
    /** \brief Constructor.
     *
     * \param b   the bot instance
     */
    Basic(Bot& b);


    /** \brief Gets a list of commands that are provided by this plugin.
     *
     * \return Returns a vector of command names implemented by this plugin.
     */
    virtual std::vector<std::string> commands() const;


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
    virtual Message handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts);


    /** \brief Gets a short, one line help text for a command
     *
     * \param command  name of the command where help is requested
     * \return Returns a short, one line help text for the command.
     */
    virtual std::string helpOneLine(const std::string_view& command) const;


    /** \brief Determines whether a command of the plugin can be deactivated.
     *
     * \param command  name of the command to deactivate
     * \return Returns true, if the command can be deactivated.
     *         Returns false, if deactivation is forbidden.
     */
    bool allowDeactivation(const std::string_view& command) const override;
  private:
    Bot& theBot; /**< reference to the bot */
}; // class

} // namespace

#endif // BVN_PLUGIN_BASIC_HPP
