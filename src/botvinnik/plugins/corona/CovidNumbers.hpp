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

#ifndef BVN_PLUGIN_COVIDNUMBERS_HPP
#define BVN_PLUGIN_COVIDNUMBERS_HPP

#include <string>
#include <vector>

namespace bvn
{

/** Corona numbers for a single day in a single country. */
struct CovidNumbersElem
{
  /** Default constructor. */
  CovidNumbersElem();

  int64_t cases;     /**< number of infections */
  int64_t deaths;    /**< number of deaths */
  double incidence14; /**< cases per 100000 inhabitants over the last 14 days */
  std::string date;  /**< date for the above numbers */
};

/** Corona numbers for several days, plus total numbers. */
struct CovidNumbers
{
  /** Default constructor. */
  CovidNumbers();

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

} // namespace

#endif // BVN_PLUGIN_COVIDNUMBERS_HPP
