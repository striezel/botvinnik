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
#include <random>
#include "XkcdData.hpp"

namespace bvn
{

Xkcd::Xkcd(Matrix& mat)
: mLatestNum(2330),
  theMatrix(mat)
{
  const auto latest = XkcdData::get(0);
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
    const auto info = XkcdData::get(num);
    if (!info.has_value() || info.value().img.empty())
    {
      return Message("Error: Could not get comic from xkcd.com!",
                     std::string("<strong>Error:</strong> Could not get comic from xkcd.com!"));
    }
    const XkcdData& data = info.value();

    const auto mxcUri = theMatrix.uploadImage(data.img);

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

} // namespace
