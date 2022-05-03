/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2022  Dirk Stolle

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

#ifndef BVN_PLUGIN_LIBRETRANSLATE_HPP
#define BVN_PLUGIN_LIBRETRANSLATE_HPP

#include "Plugin.hpp"

namespace bvn
{

/** \brief Translates text from one language to another.
 */
class LibreTranslate: public Plugin
{
  public:
    /** \brief Constructor.
     *
     * \param server  URL of the translation server,
     *                e.g. "https://libretranslate.com/"
     * \param apiKey  API key for use with the server;
     *                leave empty, if no key is required for that server
     */
    LibreTranslate(const std::string_view server, const std::string_view apiKey);


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
  private:
    std::string url; /**< server URL */
    std::string key; /**< API key for the server */


    /** \brief Gets a message containing the supported languages and their codes.
     *
     * \return Returns a message containing the supported languages and their codes.
     */
    Message getLanguages() const;


    /** \brief Handles translation for the corresponding command.
     *
     * \param command   name of the command to handle
     * \param message   complete text message that triggered the command
     * \return Returns a message to send as reply to the command.
     *         If the return value is empty, no message will be sent.
     */
    Message getTranslation(const std::string_view& command, const std::string_view& message) const;
}; // class

} // namespace

#endif // BVN_PLUGIN_LIBRETRANSLATE_HPP
