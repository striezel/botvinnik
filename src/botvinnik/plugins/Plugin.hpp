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

#ifndef BVN_PLUGIN_HPP
#define BVN_PLUGIN_HPP

#include <string>
#include <string_view>
#include <vector>

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
     * \return Returns a string that represents the message to send as reply to
     *         the command. If the return value is empty, no message will be
     *         sent.
     */
    virtual std::string handleCommand(const std::string_view& command, const std::string_view& message) = 0;
}; // class

} // namespace

#endif // BVN_PLUGIN_HPP
