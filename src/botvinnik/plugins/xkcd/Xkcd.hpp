/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#ifndef BVN_PLUGIN_XKCD_HPP
#define BVN_PLUGIN_XKCD_HPP

#include <chrono>
#include "../DeactivatablePlugin.hpp"
#include "../../../matrix/Matrix.hpp"
#include "XkcdData.hpp"

namespace bvn
{

/** \brief Replies with a random comic from xkcd.com.
 */
class Xkcd: public DeactivatablePlugin
{
  public:
    /** \brief Constructor.
     *
     * \param mat  logged in matrix instance
     */
    Xkcd(Matrix& mat);


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
  private:
    /** \brief Gets a random number denoting a valid comic.
     *
     * \param latest   latest known comic number
     * \return Returns a random number denoting a comic.
     */
    static unsigned int getRandomNumber(const unsigned int latest);

    /** \brief Extracts comic number from message, or - if none is given - gets a random number.
     *
     * \param command   name of the command to handle (usually "xkcd")
     * \param message   complete text message that triggered the command
     * \param latest   latest known comic number
     * \return Returns a number denoting a comic.
     */
    static unsigned int determineComicId(const std::string_view& command, const std::string_view& message, const unsigned int latest);

    /** \brief Uploads the comic image (if necessary) and gets its MXC URI.
     *
     * \param data   data about the comic
     * \return Returns an optional containing the MXC URI, if successful.
     *         Returns an empty optional, if an error occurred.
     */
    std::optional<std::string> uploadComic(const XkcdData& data);

    /** \brief Updates the last known comic id to the latest available id.
     */
    void updateLatestNum();

    unsigned int mLatestNum; /**< latest known comic number */
    std::chrono::steady_clock::time_point mLastUpdate; /**< time of last update of mLatestNum */
    Matrix& theMatrix; /**< reference to the Matrix instance */
}; // class

} // namespace

#endif // BVN_PLUGIN_XKCD_HPP
