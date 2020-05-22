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

#include "Matrix.hpp"
#include <iostream>
#include "../../third-party/nlohmann/json.hpp"
#include "../../third-party/simdjson/simdjson.h"
#include "../Version.hpp"
#include "../net/Curly.hpp"

namespace bvn
{

Matrix::Matrix(const Configuration& _conf)
: conf(_conf),
  accessToken(std::string())
{
}

Matrix::~Matrix()
{
  if (isLoggedIn())
    logout();
}

bool Matrix::login()
{
  if (isLoggedIn())
  {
    // In theory a new login could be attempted here, but this way we would
    // loose the old access token and leave a dangling session on the server.
    std::cerr << "Error while trying to log in: Already logged in!" << std::endl;
    return false;
  }

  /* Login JSON looks similar to:

  {
    "type": "m.login.password",
    "user": "@alice:bob.tld",
    "password": "ilovefruit",
    "initial_device_display_name": "Banana Phone"
  }
  */
  std::string response;
  {
    const nlohmann::json body = {
      { "type", "m.login.password" },
      { "user", conf.userId() },
      { "password", conf.password() },
      { "initial_device_display_name", "botvinnik, " + bvn::version }
    };
    Curly curl;
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/login");
    curl.addHeader("Content-Type: application/json");
    curl.setPostBody(body.dump());
    if (!curl.perform(response))
    {
      std::cerr << "Error: Login failed!" << std::endl
                << "HTTP status code: " << curl.getResponseCode() << std::endl
                << "Response: " << response << std::endl;
      return false;
    }
  }

  /* Response should be something like:

  {
    "user_id": "@alice:bob.tld",
    "access_token": "RpZmllciBrZXkKMDAxMGNpZCBnZW",
    "home_server": "matrix.bob.tld",
    "device_id": "QEXZJOSCPA",
    "well_known": {
      "m.homeserver": {
        "base_url": "https://matrix.bob.tld/"
      },
      "m.identity_server": {
        "base_url": "https://identity.bob.tld/"
      }
    }
  }

  In our case just the access token is required.
  */

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error while trying to log in: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  const auto [token, tokenError] = doc["access_token"];
  if (tokenError || !token.is<std::string_view>())
  {
    std::cerr << "Error while trying to log in: JSON data does not contain an access_token element string!" << std::endl;
    return false;
  }
  accessToken = std::string(token.get<std::string_view>().value());
  return true;
}

bool Matrix::logout()
{
  if (!isLoggedIn())
    return true;

  std::string response;

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/logout?access_token=" + accessToken);
  curl.setPostBody("");
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Logout failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  accessToken.clear();
  return true;
}

bool Matrix::isLoggedIn() const
{
  // If we have an access token, we must have performed a log in.
  return !accessToken.empty();
}

} // namespace
