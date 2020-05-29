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

#include "Xkcd.hpp"
#include <optional>
#include <random>
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"

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

bool endsWith(const std::string& str, const std::string& suffix)
{
  const std::string::size_type strLen = str.size();
  const std::string::size_type suffixLen = suffix.size();
  if (strLen < suffixLen)
    return false;
  return (str.substr(strLen - suffixLen).compare(suffix) == 0);
}

std::optional<XkcdData> getXkcdData(unsigned int num)
{
  std::string response;
  {
    Curly curl;
    if (num != 0)
      curl.setURL("https://xkcd.com/" + std::to_string(num) + "/info.0.json");
    else
      curl.setURL("https://xkcd.com/info.0.json");
    if (!curl.perform(response))
    {
      return std::optional<XkcdData>();
    }
  }

  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(response);
  if (parseError)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<XkcdData>();
  }

  XkcdData data;

  auto [value, error] = doc["num"];
  if (error || value.type() != simdjson::dom::element_type::INT64)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a 'num' member!" << std::endl;
    return std::optional<XkcdData>();
  }
  data.num = value.get<int64_t>();

  doc["safe_title"].tie(value, error);
  if (!error && value.type() == simdjson::dom::element_type::STRING)
  {
    data.title = value.get<std::string_view>().value();
  }
  if (data.title.empty())
  {
    doc["title"].tie(value, error);
    if (error || value.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a title!" << std::endl;
      return std::optional<XkcdData>();
    }
    data.title = value.get<std::string_view>().value();
  }

  doc["img"].tie(value, error);
  if (error || value.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain a image URL!" << std::endl;
    return std::optional<XkcdData>();
  }
  data.img = value.get<std::string_view>().value();

  doc["alt"].tie(value, error);
  if (error || value.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to parse JSON response from xkcd.com! JSON data does not contain an alternative text!" << std::endl;
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

Xkcd::Xkcd(Matrix& mat)
: mLatestNum(2312),
  theMatrix(mat)
{
  const auto latest = getXkcdData(0);
  if (latest.has_value())
  {
    mLatestNum = std::max(latest.value().num, mLatestNum);
  }
}

std::vector<std::string> Xkcd::commands() const
{
  return { "xkcd" };
}

Message Xkcd::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (command == "xkcd")
  {
    std::random_device randDev;
    std::mt19937 generator(randDev());
    std::uniform_int_distribution<unsigned int> distribution(1, mLatestNum);
    const unsigned int num = distribution(generator);

    // xkcd.info only works when server can be reached.
    const auto info = getXkcdData(num);
    if (!info.has_value() || info.value().img.empty())
    {
      return Message("Error: Could not get comic from xkcd.com!",
                     std::string("<strong>Error:</strong> Could not get comic from xkcd.com!"));
    }
    const XkcdData& data = info.value();

    const auto mxcUri = uploadImage(data.img);

    Message xkcd;
    // Create normal body - varies, when transcript is there.
    xkcd.body = "xkcd.com # " + std::to_string(data.num) + ": " + data.title
              + "\n\n";
    if (!data.transcript.empty())
    {
      xkcd.body.append(data.transcript).append("\n\n");
    }
    else
    {
      xkcd.body.append(data.img).append("\n\n").append(data.alt).append("\n\n");
    }
    xkcd.body.append("Source: https://xkcd.com/" + std::to_string(data.num) + "/");
    // formatted body
    if (mxcUri.has_value())
    {
      // Use mxc URI for image.
      xkcd.formatted_body = "xkcd.com # " + std::to_string(data.num) + ": <strong>" + data.title
              + "</strong><br />\n<br />\n<img src=\"" + mxcUri.value() + "\" alt=\"\"><br >\n"
              + "<br /><em>" + data.alt + "</em><br />Source: https://xkcd.com/"
              + std::to_string(data.num) + "/";
    }
    else
    {
      // Upload failed. Use original URL as fallback.
      xkcd.formatted_body = "xkcd.com # " + std::to_string(data.num) + ": <strong>" + data.title
              + "</strong><br />\n<br />\n" + data.img + "<br >\n"
              + "<br /><em>" + data.alt + "</em><br />Source: https://xkcd.com/"
              + std::to_string(data.num) + "/";
    }
    return xkcd;
  }

  // unknown command
  return Message();
}

std::string Xkcd::helpOneLine(const std::string_view& command) const
{
  if (command == "xkcd")
  {
    return "displays a random comic from xkcd.com";
  }

  return std::string();
}

std::optional<std::string> Xkcd::uploadImage(const std::string& imgUrl)
{
  std::string imageData;
  {
    Curly curl;
    curl.setURL(imgUrl);
    std::clog << "Info: Downloading image " << imgUrl << "..." << std::endl;
    if (!curl.perform(imageData) || curl.getResponseCode() != 200)
    {
      std::cerr << "Error: Could get image from " + imgUrl + "!" << std::endl
                << "HTTP status code: " << curl.getResponseCode() << std::endl
                << "Response: " << imageData << std::endl;
      return std::optional<std::string>();
    }
  }

  std::string contentType("application/octet-stream");
  if (endsWith(imgUrl, ".png"))
    contentType = "image/png";
  else if (endsWith(imgUrl, ".jpg") || endsWith(imgUrl, ".jpeg"))
    contentType = "image/jpeg";
  else if (endsWith(imgUrl, ".gif"))
    contentType = "image/gif";

  std::string fileName;
  const std::string::size_type pos = imgUrl.rfind('/');
  if ((pos != std::string::npos) && (imgUrl.size() > pos + 1))
    fileName = imgUrl.substr(pos + 1);
  else
    fileName = "image.data";

  std::clog << "Info: Uploading image " << imgUrl << " ("
            << static_cast<long int>(imageData.size())
            << " bytes) to Matrix ..." << std::endl;
  return theMatrix.uploadString(imageData, contentType, fileName);
}

} // namespace
