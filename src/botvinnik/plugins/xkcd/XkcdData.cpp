/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#include "XkcdData.hpp"
#include <iostream>
#include "../../../../third-party/simdjson/simdjson.h"
#include "../../../net/Curly.hpp"

namespace bvn
{

XkcdData::XkcdData()
: num(0),
  title(""),
  img(""),
  transcript(""),
  alt("")
{
}

std::optional<XkcdData> XkcdData::get(unsigned int num)
{
  std::string response;
  {
    Curly curl;
    if (num != 0)
    {
      curl.setURL("https://xkcd.com/" + std::to_string(num) + "/info.0.json");
    }
    else
    {
      curl.setURL("https://xkcd.com/info.0.json");
    }
    if (!curl.perform(response) || curl.getResponseCode() != 200)
    {
      return std::optional<XkcdData>();
    }
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto parseError = parser.parse(response).get(doc);
  if (parseError)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com!\n"
              << "Response is: " << response << std::endl;
    return std::optional<XkcdData>();
  }

  XkcdData data;

  simdjson::dom::element value;
  auto error = doc["num"].get(value);
  if (error || value.type() != simdjson::dom::element_type::INT64)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a 'num' member!\n";
    return std::optional<XkcdData>();
  }
  data.num = value.get<int64_t>();

  doc["safe_title"].tie(value, error);
  if (!error && value.type() == simdjson::dom::element_type::STRING)
  {
    data.title = value.get<std::string_view>().value();
  }
  // Fall back to "normal" title, if there is no safe_title element.
  if (data.title.empty())
  {
    doc["title"].tie(value, error);
    if (error || value.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a title!\n";
      return std::optional<XkcdData>();
    }
    data.title = value.get<std::string_view>().value();
  }

  doc["img"].tie(value, error);
  if (error || value.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a image URL!\n";
    return std::optional<XkcdData>();
  }
  data.img = value.get<std::string_view>().value();

  doc["alt"].tie(value, error);
  if (error || value.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain an alternative text!\n";
    return std::optional<XkcdData>();
  }
  data.alt = value.get<std::string_view>().value();

  doc["transcript"].tie(value, error);
  if (!error && value.type() == simdjson::dom::element_type::STRING)
  {
    data.transcript = value.get<std::string_view>().value();
  }

  return std::optional<XkcdData>(data);
}

} // namespace
