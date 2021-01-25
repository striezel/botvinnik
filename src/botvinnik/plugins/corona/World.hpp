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

#ifndef BVN_PLUGINS_CORONA_WORLD_HPP
#define BVN_PLUGINS_CORONA_WORLD_HPP

#include <optional>
#include "CovidNumbers.hpp"

namespace bvn
{

struct World
{
  private:
    /**
     * Gets all available countries.
     */
    static std::vector<Country> all();
  public:
    /** \brief Finds country by name or ISO 3166 two letter code.
     *
     * \param nameOrGeoId  name OR ISO 3166 two letter code of the country
     * \return Returns an optional containing the matching country.
     *         Returns an empty optional, if no match was found.
     */
    static std::optional<Country> find(const std::string& nameOrGeoId);

    /** \brief Gets recent data for a given country.
     *
     * \param country   the country for which the data shall be retrieved.
     * \return Returns CovidNumbers with data, if successful.
     *         Returns an empty CovidNumbers instance otherwise.
     */
    static CovidNumbers getCountryData(const Country& country);
};

} // namespace

#endif // BVN_PLUGINS_CORONA_WORLD_HPP
