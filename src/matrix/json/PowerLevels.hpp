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

#ifndef BVN_MATRIX_JSON_POWERLEVELS_HPP
#define BVN_MATRIX_JSON_POWERLEVELS_HPP

#include <optional>
#include "../../../third-party/simdjson/simdjson.h"
#include "../events/PowerLevels.hpp"

namespace bvn::matrix::json
{

/** \brief Parses the power level response from the matrix client-server API.
 *
 * \param json   the JSON as plain text
 * \return Returns an optional containing parsed power levels.
 *         Returns an empty optional, if the parsing failed.
 */
std::optional<PowerLevels> parsePowerLevels(const std::string& json);

} // namespace

#endif // BVN_MATRIX_JSON_POWERLEVELS_HPP
