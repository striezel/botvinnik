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

#include "Plugin.hpp"
#include <chrono>
#include <optional>

namespace bvn
{

/** Corona numbers for a single day in a single country. */
struct CovidNumbersElem
{
  int64_t cases;     /**< number of infections */
  int64_t deaths;    /**< number of deaths */
  std::string date;  /**< date for the above numbers */
};

/** Corona numbers for several days, plus total numbers. */
struct CovidNumbers
{
  int64_t totalCases;             /**< total number of infections since start */
  int64_t totalDeaths;            /**< total number of deaths since start */
  std::vector<CovidNumbersElem> days;  /**< daily numbers */

  /** \brief Gets the percentage of deaths, rounded to two decimals, as string.
   *
   * \return Returns the percentage of deaths.
   */
  std::string percentage() const;
};

struct Country
{
  /** \brief Constructor.
   *
   * \param id     numeric countryId from database
   * \param _name  name of the country
   * \param _geoId geoId from the database / ISO-3166 two letter code
   */
  Country(const int64_t id = -1, const std::string& _name = "", const std::string& _geoId = "");

  int64_t countryId;
  std::string name;
  std::string geoId;
};

/** \brief Show current case numbers for corona virus.
 */
class Corona: public Plugin
{
  public:
    /** \brief Constructor.
     */
    Corona();


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
     * \param server_ts timestamp when the message reached the server
     * \return Returns a message to send as reply to the command.
     *         If the return value is empty, no message will be sent.
     */
    virtual Message handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts);


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
    std::optional<std::pair<std::string, std::chrono::steady_clock::time_point> > dbLocation;

    static std::optional<std::string> buildDatabase(const std::string& csv);
}; // class

} // namespace

#endif // BVN_PLUGIN_CORONA_HPP