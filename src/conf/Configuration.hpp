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

#ifndef BVN_CONFIGURATION_HPP
#define BVN_CONFIGURATION_HPP

#include <map>
#include <vector>

namespace bvn
{

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


    /** \brief Finds the first existing task directory from a preset list.
     *
     * \param realName  directory name, if any
     */
    void findTaskDirectory(std::string& realName);

    std::string mHomeServer; /**< Matrix home server */
    std::string mUserId; /**< Matrix user id used for login */
    std::string mPassword; /**< password used for login */
}; // class

} // namespace

#endif // BVN_CONFIGURATION_HPP
