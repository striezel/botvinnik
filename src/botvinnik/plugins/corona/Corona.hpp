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

#ifndef BVN_PLUGIN_CORONA_HPP
#define BVN_PLUGIN_CORONA_HPP

#include "../Plugin.hpp"
#include <chrono>
#include <optional>
#include "../../../matrix/Matrix.hpp"

namespace bvn
{

/** \brief Show current case numbers for corona virus.
 */
class Corona: public Plugin
{
  public:
    /** \brief Constructor.
     *
     * \param matrix the Matrix client instance
     */
    Corona(Matrix& matrix);


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


    /** \brief Creates a sqlite database with corona virus case numbers.
     *
     * \return Returns an optional containing the path of the created database.
     *         Returns an empty optional, if an error occurred.
     */
    static std::optional<std::string> createDatabase();
  private:
    std::optional<std::pair<std::string, std::chrono::steady_clock::time_point> > dbLocation; /**< location and update time of the database file */
    Matrix& theMatrix; /**< reference to the Matrix client */


    /** \brief Builds a new SQLite database from the CSV contents.
     *
     * \param csv   content of the CSV file
     * \return Returns an optional containing the path to the created SQLite database.
     * Returns an empty optional, if database creation failed.
     */
    static std::optional<std::string> buildDatabase(const std::string& csv);
}; // class

} // namespace

#endif // BVN_PLUGIN_CORONA_HPP
