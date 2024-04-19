/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2023  Dirk Stolle

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
#include <iostream>
#include <random>
#include "../../../third-party/nonstd/expected.hpp"
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"
#include "../../net/url_encode.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

Giphy::Giphy(const std::string& gifApiKey, Matrix& mat)
: apiKey(gifApiKey),
  theMatrix(mat)
{
  trim(this->apiKey);
}

std::vector<std::string> Giphy::commands() const
{
  return { "giphy" };
}

Message Giphy::handleCommand(const std::string_view& command, const std::string_view& message,
                             [[maybe_unused]] const std::string_view& userId,
                             const std::string_view& roomId,
                             [[maybe_unused]] const std::chrono::milliseconds& server_ts)
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

    return performQuery(query, roomId);
  }

  // unknown command
  return Message();
}

std::string Giphy::helpOneLine(const std::string_view& command) const
{
  if (command == "giphy")
  {
    return "displays a random image from giphy.com matching the given keyword";
  }

  return std::string();
}

Message Giphy::helpExtended(const std::string_view& command, const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "giphy")
  {
    return Message("show a random GIF file matching a given keyword. For example, `"s
        .append(prefix) + "giphy cat` will show a random GIF of a cat.",
        "show a random GIF file matching a given keyword. For example, <code>"s
        .append(prefix) + "giphy cat</code> will show a random GIF of a cat.");
  }

  return Message();
}

struct GiphyImageData
{
  std::string url; /**< URL of image in CDN server */
  ImageInfo info;  /**< metadata for the image */
};

nonstd::expected<std::vector<GiphyImageData>, Message> extract_images_from_json(const std::string& json)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto parseError = parser.parse(json).get(doc);
  if (parseError)
  {
    std::cerr << "Error while trying to parse JSON response from Giphy API!" << std::endl
              << "Response is: " << json << std::endl;
    return nonstd::make_unexpected(Message("The API request to get information from Giphy failed (invalid JSON)."));
  }
  simdjson::dom::element data;
  const auto dataError = doc.at_pointer("/data").get(data);
  if (dataError || data.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error: JSON response from Giphy does not contain data array!" << std::endl
              << "Response is: " << json << std::endl;
    return nonstd::make_unexpected(Message("The request to get images from Giphy failed. Giphy server returned unexpected JSON format."));
  }

  std::vector<GiphyImageData> result;

  for (const auto & item : data)
  {
    simdjson::dom::element item_element;
    auto error = item.at_pointer("/images/original/url").get(item_element);
    if (error || item_element.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error: JSON response from Giphy does not contain '/images/original/url' element!" << std::endl
                << "Response is: " << json << std::endl;
      return nonstd::make_unexpected(Message("The request to get images from Giphy failed. Giphy server returned unexpected JSON format."));
    }
    GiphyImageData current_data;
    current_data.url = std::string(item_element.get<std::string_view>().value());
    // It's always GIF for the default image.
    current_data.info.mimeType = "image/gif";

    error = item.at_pointer("/images/original/width").get(item_element);
    // Despite being an integer value, the API returns a string containing the
    // width value (e. g. "400") instead of the real value (e. g. 400).
    if (error || !item_element.is_string())
    {
      std::clog << "Error: JSON response from Giphy does not contain '/images/original/width' element!" << std::endl
                << "Response is: " << json << std::endl;
    }
    else
    {
      const auto string = item_element.get<std::string_view>().value();
      int value = 0;
      if (stringToInt(std::string(string), value))
      {
        current_data.info.width = value;
      }
    }

    error = item.at_pointer("/images/original/height").get(item_element);
    if (error || !item_element.is_string())
    {
      std::clog << "Error: JSON response from Giphy does not contain '/images/original/width' element!" << std::endl
                << "Response is: " << json << std::endl;
    }
    else
    {
      const auto string = item_element.get<std::string_view>().value();
      int value = 0;
      if (stringToInt(std::string(string), value))
      {
        current_data.info.height = value;
      }
    }

    error = item.at_pointer("/images/original/size").get(item_element);
    if (error || !item_element.is_string())
    {
      std::clog << "Error: JSON response from Giphy does not contain '/images/original/width' element!" << std::endl
                << "Response is: " << json << std::endl;
    }
    else
    {
      const auto string = item_element.get<std::string_view>().value();
      int value = 0;
      if (stringToInt(std::string(string), value))
      {
        current_data.info.size = value;
      }
    }

    result.emplace_back(current_data);
  }

  return result;
}

Message Giphy::performQuery(const std::string& query, const std::string_view& roomId)
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

  const auto maybe_images = extract_images_from_json(response);
  if (!maybe_images.has_value())
  {
    return maybe_images.error();
  }
  const auto& images = maybe_images.value();
  if (images.empty())
  {
    return Message("The search request returned no matching GIFs.");
  }

  std::random_device randDev;
  std::mt19937 generator(randDev());
  std::uniform_int_distribution<unsigned int> distribution(0, images.size() - 1);
  const unsigned int num = distribution(generator);


  const auto mxcUri = theMatrix.uploadImage(images[num].url);
  if (!mxcUri.has_value())
  {
    return Message("Failed to upload GIF to Matrix homeserver.");
  }

  if (!theMatrix.sendImage(std::string(roomId), mxcUri.value(), "giphy.gif", images[num].info))
  {
    return Message("Could not send GIF to Matrix server.");
  }

  Message gif;
  gif.formatted_body = "<em>Powered by GIPHY</em>";
  gif.body = "Powered by GIPHY";
  return gif;
}

} // namespace
