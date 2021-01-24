/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2021  Dirk Stolle

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

#ifndef BVN_PLUGIN_CORONA_DISEASE_SH_HPP
#define BVN_PLUGIN_CORONA_DISEASE_SH_HPP

#include "CovidNumbers.hpp"

namespace bvn
{

namespace disease_sh
{

/**
 * Request historical API of disease.sh for a single country.
 *
 * @param  geoId  geo id (i. e. two letter country code) of a country
 * @param  all    whether to collect recent or all data
 * @return Returns a CovidNumbers containing the new daily cases.
 *         If an error occurred, an empty CovidNumbers struct is returned.
 */
CovidNumbers requestHistoricalApi(const std::string& geoId, const bool all);

/**
 * Request historical API of disease.sh for a single province of a country.
 *
 * @param  geoId  geo id (i. e. two letter country code) of a country
 * @param  province  name of the province as seen in the API response for the country
 * @param  all   whether to collect recent or all data
 * @return Returns a CovidNumbers containing the new daily cases.
 *         If an error occurred, an empty CovidNumbers struct is returned.
 */
CovidNumbers requestHistoricalApiProvince(const std::string& geoId, const std::string& province, const bool all);

} // namespace

} // namespace

#endif // BVN_PLUGIN_CORONA_DISEASE_SH_HPP
