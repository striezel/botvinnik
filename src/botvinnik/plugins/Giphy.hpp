/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2023, 2024  Dirk Stolle

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

#ifndef BVN_PLUGIN_GIPHY_HPP
#define BVN_PLUGIN_GIPHY_HPP

#include "DeactivatablePlugin.hpp"
#include "../../matrix/Matrix.hpp"

namespace bvn
{

/** \brief Replies with a random GIF from giphy.com.
 */
class Giphy final: public DeactivatablePlugin
{
  public:
    /** \brief Constructor.
     *
     * \param apiKey  key for the Giphy API
     * \param mat  logged in matrix instance
     */
    Giphy(const std::string& apiKey, Matrix& mat);


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


    /** \brief Gets a short, one line help text for a command
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
    Message performQuery(const std::string& query, const std::string_view& roomId);

    std::string apiKey; /**< Giphy API key */
    Matrix& theMatrix;  /**< reference to the Matrix instance */
}; // class

} // namespace

#endif // BVN_PLUGIN_XKCD_HPP
