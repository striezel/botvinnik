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


#include "PowerLevels.hpp"
#include "../../../third-party/simdjson/simdjson.h"

namespace bvn::matrix::json
{

std::optional<PowerLevels> parsePowerLevels(const std::string& json)
{
  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(json);
  if (error)
  {
    std::cerr << "Error while parsing power levels: Unable to parse JSON data!" << std::endl
              << "JSON is: " << json << std::endl;
    return std::optional<matrix::PowerLevels>();
  }

  matrix::PowerLevels result;
  simdjson::dom::element elem;
  simdjson::error_code err;
  doc["ban"].tie(elem, err);
  if (!err && elem.type() == simdjson::dom::element_type::INT64)
  {
    result.ban = elem.get<int64_t>();
  }
  doc["kick"].tie(elem, err);
  if (!err && elem.type() == simdjson::dom::element_type::INT64)
  {
    result.kick = elem.get<int64_t>();
  }
  doc["users_default"].tie(elem, err);
  if (!err && elem.type() == simdjson::dom::element_type::INT64)
  {
    result.users_default = elem.get<int64_t>();
  }
  const auto [users, errUsers] = doc["users"];
  if (!errUsers && users.type() == simdjson::dom::element_type::OBJECT)
  {
    simdjson::dom::object usersObject;
    users.get<simdjson::dom::object>().tie(usersObject, err);
    if (err)
    {
      std::cerr << "Error: 'users' is not an object!" << std::endl;
      return std::optional<matrix::PowerLevels>();
    }

    // iterate
    for (const auto [key, value] : usersObject)
    {
      if (value.type() != simdjson::dom::element_type::INT64)
      {
        std::cerr << "Error: 'users' is not an object with string keys and integer values!" << std::endl;
        return std::optional<matrix::PowerLevels>();
      }
      result.users[std::string(key)] = value.get<int64_t>();
    }
  }

  return result;
}

} // namespace