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

#ifndef BVN_CONFIGURATION_HPP
#define BVN_CONFIGURATION_HPP

#include <chrono>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

namespace bvn
{

/** \brief Contains and loads configuration settings for the bot.
 */
class Configuration
{
  public:
    /** \brief Constructor.
     */
    Configuration();


    /** \brief character that indicates comment line in configuration file
     */
    static const char commentCharacter;


    /** \brief Gets a list of potential configuration file names.
     *
     * \return Returns a list of potential configuration file names.
     */
    static std::vector<std::string> potentialFileNames();


    /** \brief Gets the current home server.
     *
     * \return Returns the current home server.
     */
    const std::string& homeServer() const;


    /** \brief Gets the user id for login.
     *
     * \return Returns the user id for login.
     */
    const std::string& userId() const;


    /** \brief Gets the user's password for login.
     *
     * \return Returns the password for login.
     */
    const std::string& password() const;


    /** \brief Gets the prefix for bot commands.
     *
     * \return Returns the prefix for bot commands.
     */
    const std::string& prefix() const;


    /** \brief Gets a set of commands that will be deactivated.
     *
     * \return Returns a set of bot commands that will be deactivated.
     */
    const std::unordered_set<std::string>& deactivatedCommands() const;


    /** \brief Gets a set of user ids that may stop the bot.
     *
     * \return Returns a set of user ids for users that are allowed to stop the
     *         bot.
     */
    const std::unordered_set<std::string>& stopUsers() const;


    /** \brief Checks whether a given user is allowed to manage the bot.
     *
     * \param userId   id of the user to check
     * \return Returns true, if the user is allowed to manage the bot.
     *         Returns false otherwise.
     */
    bool isAdminUser(const std::string& userId) const;


    /** \brief Gets the number of allowed failures for Matrix sync requests.
     *
     * \return Returns the number of allowed failures for Matrix sync requests.
     */
    int allowedFailures() const;


    /** \brief Gets the delay between two consecutive sync requests.
     *
     * \return Returns the delay between two consecutive sync requests.
     *         If no delay has been set yet, it returns zero.
     */
    std::chrono::milliseconds syncDelay() const;


    /** \brief minimal allowed synchronization delay
     */
    static const std::chrono::milliseconds min_sync_delay;


    /** \brief default synchronization delay
     */
    static const std::chrono::milliseconds default_sync_delay;


    /** \brief maximal allowed synchronization delay
     */
    static const std::chrono::milliseconds max_sync_delay;


    /** \brief Gets the URL of the translation server.
     *
     * \return Returns the URL of the translation server.
     * \remarks This may be empty, if no URL was set.
     */
    const std::string& translationServer() const;


    /** \brief Gets the API key of the translation server.
     *
     * \return Returns the API key of the translation server.
     * \remarks This may be empty, if no key was set.
     */
    const std::string& translationApiKey() const;


    /** \brief Loads the configuration from a file.
     *
     * \param  fileName   file name of the configuration file
     * \return Returns true, if configuration was loaded successfully.
     *         Returns false otherwise.
     */
    bool load(const std::string& fileName = std::string());


    /** \brief Resets all configuration values to empty values.
     */
    void clear();
  private:
    /** \brief Finds the first existing configuration file from a preset list.
     *
     * \param realName  name of the configuration file, if any
     */
    void findConfigurationFile(std::string& realName);


    /** \brief Loads the configuration file with "core" settings.
     *
     * \param fileName  file name of the configuration file
     * \return Returns true, if file was loaded successfully.
     */
    bool loadCoreConfiguration(const std::string& fileName);

    std::string mHomeServer; /**< Matrix home server */
    std::string mUserId; /**< Matrix user id used for login */
    std::string mPassword; /**< password used for login */
    std::string mPrefix; /**< prefix for commands */
    std::unordered_set<std::string> mDeactivatedCommands; /**< commands that will be deactivated */
    std::unordered_set<std::string> mStopUsers; /**< users that may stop the bot */
    int mAllowedFailsIn64; /**< allowed sync failures in 64 attempts */
    std::chrono::milliseconds mSyncDelay; /**< delay between two consecutive sync requests */

    // plugin-related settings
    std::string mLibreTranslateServer; /**< URL of the LibreTranslate server */
    std::string mLibreTranslateApiKey; /**< API key for the LibreTranslate server */
}; // class

} // namespace

#endif // BVN_CONFIGURATION_HPP
