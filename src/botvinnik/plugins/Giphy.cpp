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

#include "Giphy.hpp"
#include <optional>
#include <random>
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"
#include "../../net/url_encode.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

Giphy::Giphy(const std::string& apiKey, Matrix& mat)
: apiKey(apiKey),
  theMatrix(mat)
{
  trim(this->apiKey);
}

std::vector<std::string> Giphy::commands() const
{
  return { "giphy" };
}

Message Giphy::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (command == "giphy")
  {
    std::string query(message.substr(command.size()));
    trim(query);
    if (query.empty())
    {
      return Message(std::string("Please enter a keyword for the search after '").append(command).append("'."));
    }
    if (apiKey.empty())
    {
      return Message("No API key for Giphy has been configured, so no images can be retrieved.");
    }

    return performQuery(query);
  }

  // unknown command
  return Message();
}

std::string Giphy::helpOneLine(const std::string_view& command) const
{
  if (command == "Giphy")
  {
    return "displays a random image from giphy.com";
  }

  return std::string();
}

Message Giphy::performQuery(const std::string& query)
{
  std::string encodedQuery;
  std::string encodedKey;
  try
  {
    encodedQuery = urlencode(query);
    encodedKey = urlencode(apiKey);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Failed to URL-encode Giphy query!\n" << ex.what() << std::endl;
    return Message("Could not request image from Giphy.");
  }

  Curly curl;
  curl.setURL("https://api.giphy.com/v1/gifs/search?api_key=" + encodedKey + "&q=" + encodedQuery + "&limit=25&offset=0&rating=G&lang=en");
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to Giphy API failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The API request to get information from Giphy failed.");
  }

  // parse JSON
  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(response);
  if (parseError)
  {
    std::cerr << "Error while trying to parse JSON response from Giphy API!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The API request to get information from Giphy failed (invalid JSON).");
  }


    std::random_device randDev;
    std::mt19937 generator(randDev());
    std::uniform_int_distribution<unsigned int> distribution(1, 25);
    const unsigned int num = distribution(generator);

  return Message("Error: Giphy is not implemented yet.");
}

} // namespace
