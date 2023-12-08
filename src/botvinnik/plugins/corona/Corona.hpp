/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022, 2023  Dirk Stolle

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

#include "../DeactivatablePlugin.hpp"
#include "../../../matrix/Matrix.hpp"

namespace bvn
{

/** \brief Show current case numbers for corona virus.
 */
class Corona final: public DeactivatablePlugin
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
    std::optional<std::pair<std::string, std::chrono::steady_clock::time_point> > dbLocation; /**< location and update time of the database file */
    Matrix& theMatrix; /**< reference to the Matrix client */


    /** \brief Creates an SQLite database with corona virus case numbers.
     *
     * \return Returns an optional containing the path of the created database.
     *         Returns an empty optional, if an error occurred.
     */
    static std::optional<std::string> createDatabase();


    /** \brief Builds a new SQLite database from the CSV contents.
     *
     * \param csv   content of the CSV file
     * \return Returns an optional containing the path to the created SQLite database.
     *         Returns an empty optional, if database creation failed.
     */
    static std::optional<std::string> buildDatabase(const std::string& csv);


    /** \brief Update existing database if it is outdated, or create database if none exists yet.
     *
     * \param roomId    id of the room that was used to invoke the command
     * \return Returns an empty optional, if operation was successful.
     *         Returns an optional containing a usable error message in case of
     *         failure. This message can be used to send it to the current
     *         Matrix room and inform the user of the failure.
     */
    std::optional<Message> updateOldDatabase(const std::string_view& roomId);
}; // class

} // namespace

#endif // BVN_PLUGIN_CORONA_HPP
