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

#ifndef BVN_BOT_HPP
#define BVN_BOT_HPP

#include <functional>
#include <unordered_map>
#include "../conf/Configuration.hpp"
#include "../matrix/Matrix.hpp"
#include "plugins/Plugin.hpp"

namespace bvn
{

// forward declarations
class Basic;
class Help;
class Rooms;

/** \brief Basic class to handle bot behaviour.
 */
class Bot
{
  public:
    /** \brief Creates a new bot instance.
     *
     * \param conf   a loaded bot configuration
     */
    Bot(const Configuration& conf);


    /** \brief Registers a plugin for use with the bot.
     *
     * A successfully registered plugin is active and incoming commands will be
     * dispatched to it.
     *
     * \param plug   the plugin to register
     * \return Returns whether the plugin was registered successfully.
     */
    bool registerPlugin(Plugin& plug);


    /** \brief Handles all configured command deactivations.
     *
     * \return Returns whether the deactivations were successful.
     * \remarks Only call this once, and only after all plugins have been
     *          registered. Otherwise this method might fail, because only the
     *          commands of registered plugins can be deactivated.
     */
    bool handleCommandDeactivations();


    /** \brief Starts the bot.
     *
     * \remarks Any plugins / commands have to be registered before the start.
     */
    void start();


    /** \brief Requests the bot to stop command processing.
     *
     * \param userId  id of the Matrix user that requested to stop the bot
     * \return Returns true, if user is allowed to stop the bot and a stop has
     *         been initiated.
     * \remark The bot will usually stop after the current requests have been
     *         processed, i. e. it may not stop immediately.
     */
    bool stop(const std::string_view& userId);


    /** \brief Whether the bot has been asked to stop.
     *
     * \return Returns true, if a stop has been requested.
     */
    bool stopRequested() const;


    /** \brief Gets reference to the matrix instance.
     *
     * \return Returns a reference to the matrix instance.
     */
    Matrix& matrix();


    // Help class needs to iterate over registered commands and plugins of the
    // bot, i. e. it needs access to private data.
    friend Help;
  private:
    /** \brief Handles joining of a Matrix room for the bot.
     *
     * \param roomId   id of the room
     */
    void joinRoom(const std::string& roomId);

    /** \brief Handles Matrix room events received via the sync endpoint.
     *
     * \param prefix  prefix for commands, e. g. "!"
     * \param rooms   vector of rooms with their events
     */
    void handleRoomEvents(const std::string& prefix, const std::vector<matrix::Room>& rooms);

    Matrix mat; /**< handles matrix requests */
    std::unordered_map<std::string, std::reference_wrapper<Plugin> > commands; /**< registered commands */
    bool stopped; /**< whether the bot shall stop */
}; // class

} // namespace

#endif // BVN_BOT_HPP
