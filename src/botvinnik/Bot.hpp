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

#ifndef BVN_BOT_HPP
#define BVN_BOT_HPP

#include <functional>
#include <unordered_map>
#include "../conf/Configuration.hpp"
#include "../matrix/Matrix.hpp"
#include "plugins/Plugin.hpp"

namespace bvn
{

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

    /** \brief Destructor.
     */
    ~Bot();

    /** \brief Registers a plugin for use with the bot.
     *
     * A successfully registered plugin is active and incoming commands will be
     * dispatched to it.
     *
     * \param plug   the plugin to register
     * \return Returns whether the plugin was registered successfully.
     */
    bool registerPlugin(Plugin& plug);


    /** \brief Deregisters a plugin command.
     *
     * A deregistered command will become inactive and the bot will no longer
     * process incoming requests for it.
     *
     * \param cmd  the plugin command to deregister
     * \return Returns whether the deregistration was successful.
     */
    bool deregisterPluginCommand(const std::string& cmd);


    /** \brief Starts the bot.
     *
     * \remarks Any plugins / commands have to be registered before the start.
     */
    void start();


    /** \brief Requests the bot to stop command processing.
     *
     * The bot will usually stop after the current requests have been processed,
     * i. e. it may not stop immediately.
     */
    void stop();


    /** \brief Whether the bot has been asked to stop.
     *
     * \return Returns true, if a stop has been requested.
     */
    bool stopRequested() const;
  private:
    Matrix mat; /**< handles matrix requests */
    std::unordered_map<std::string, std::reference_wrapper<Plugin> > commands; /**< registered commands */
    bool stopped; /**< whether the bot shall stop */
}; // class

} // namespace

#endif // BVN_BOT_HPP
