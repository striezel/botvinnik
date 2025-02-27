/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2023, 2025  Dirk Stolle

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

#include <cstdint>
#include <string>
#include <vector>

namespace bvn
{

/** Corona numbers for a single day in a single country. */
struct CovidNumbersElem
{
  /** Default constructor. */
  CovidNumbersElem();

  /**
   * Constructor with initial values.
   *
   * @param _cases  number of cases on that day
   * @param _deaths number of deaths
   * @param _incidence7 incidence value, or NaN if unknown
   * @param _data   date (e.g. "2020-12-31")
   */
  CovidNumbersElem(int64_t _cases, int64_t _deaths, double _incidence7, const std::string& _date);

  int64_t cases;     /**< number of infections */
  int64_t deaths;    /**< number of deaths */
  double incidence7; /**< cases per 100000 inhabitants over the last seven days */
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
   * \param _geoId geoId from the database / ISO-3166 three letter code
   * \param pop    population of the country
   */
  Country(const int64_t id = -1, const std::string& _name = "", const std::string& _geoId = "", const int64_t pop = -1);

  int64_t countryId;
  std::string name;
  std::string geoId;
  int64_t population;
};

/**
 * Calculates the seven-day incidence for a vector of numbers that are pre-sorted
 * by date in ascending order.
 *
 * @param number  vector of numbers, has to be sorted by date in ascending order
 *                without any gaps
 * @param population  number of inhabitants in the country
 * @return Returns the numbers with seven-day incidence calculated.
 */
std::vector<CovidNumbersElem> calculate_incidence(const std::vector<CovidNumbersElem>& numbers, const int32_t population);

} // namespace

#endif // BVN_PLUGIN_COVIDNUMBERS_HPP
