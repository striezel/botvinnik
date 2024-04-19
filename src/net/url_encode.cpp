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

#include "url_encode.hpp"
#include <curl/curl.h>
#include <stdexcept>

namespace bvn
{

std::string urlencode(const std::string_view& str)
{
  if (str.empty())
  {
    return "";
  }

  // The easiest way to encode a string is to fire up a curl_easy instance and
  // let it do the work. It may not be the fastest way, though, because a new
  // curl handle is created and destroyed every time.

  CURL* curl = curl_easy_init();
  if (curl == nullptr)
  {
    // Something just went wrong with curl.
    curl_easy_cleanup(curl);
    throw std::runtime_error("Failed to initialize curl library handle!");
  }

  char* escaped = curl_easy_escape(curl, str.data(), str.size());
  if (escaped == nullptr)
  {
    curl_easy_cleanup(curl);
    throw std::runtime_error("Could not escape string with curl!");
  }

  const std::string result(escaped);
  curl_free(escaped);
  curl_easy_cleanup(curl);
  return result;
}

}
