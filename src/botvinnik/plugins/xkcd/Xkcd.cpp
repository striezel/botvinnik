/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022, 2023, 2024  Dirk Stolle

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
#include <iostream>
#include <random>
#include "XkcdDb.hpp"
#include "../../../net/htmlspecialchars.hpp"
#include "../../../util/Strings.hpp"

namespace bvn
{

Xkcd::Xkcd(Matrix& mat)
: mLatestNum(2924),
  mLastUpdate(std::chrono::steady_clock::now() - std::chrono::hours(24)),
  theMatrix(mat)
{
  updateLatestNum();
}

void Xkcd::updateLatestNum()
{
  const auto latest = XkcdData::get(0);
  if (latest.has_value())
  {
    mLatestNum = std::max(latest.value().num, mLatestNum);
    mLastUpdate = std::chrono::steady_clock::now();
  }
}

std::vector<std::string> Xkcd::commands() const
{
  return { "xkcd" };
}

unsigned int Xkcd::getRandomNumber(const unsigned int latest)
{
  std::random_device randDev;
  std::mt19937 generator(randDev());
  std::uniform_int_distribution<unsigned int> distribution(1, latest);
  return distribution(generator);
}

unsigned int Xkcd::determineComicId(const std::string_view& command, const std::string_view& message, const unsigned int latest)
{
  if (message.size() > command.size() + 1)
  {
    std::string number(message.substr(command.size()));
    trim(number);
    unsigned long value;
    std::size_t pos = 0;
    try
    {
      value = std::stoul(number, &pos);
      if (pos == number.size() && value > 0 && value <= latest)
      {
         return value;
      }
    }
    catch (const std::exception& ex)
    {
      // Nothing to do here.
    }
  }

  return getRandomNumber(latest);
}

std::optional<std::string> Xkcd::uploadComic(const XkcdData& data)
{
  auto db = XkcdDb::getDatabase();
  std::optional<std::string> mxcUri;
  if (db.has_value())
  {
    mxcUri = XkcdDb::getMxcUri(db.value(), data.num);
    if (!mxcUri.has_value())
    {
      // No existing media. Upload it and save it in DB.
      mxcUri = theMatrix.uploadImage(data.img);
      if (mxcUri.has_value())
      {
        if (XkcdDb::insertMxcUri(db.value(), data.num, mxcUri.value()))
        {
          std::clog << "Info: Inserted MXC URI for comic #" << data.num << " into database." << std::endl;
        }
        else
        {
          std::clog << "Warning: Failed to insert MXC URI for comic #" << data.num << " into database!" << std::endl;
        }
      }
    }
  }
  else
  {
    // Database error occurred or DB does not exist. Fallback to old approach
    // and upload new media every time.
    mxcUri = theMatrix.uploadImage(data.img);
  }
  return mxcUri;
}

Message Xkcd::handleCommand(const std::string_view& command, const std::string_view& message,
                            [[maybe_unused]] const std::string_view& userId,
                            [[maybe_unused]] const std::string_view& roomId,
                            [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if (command != "xkcd")
  {
    // unknown command
    return Message();
  }

  // Check for newer comic id, if it is older than 12 hours.
  if (mLastUpdate + std::chrono::hours(12) < std::chrono::steady_clock::now())
  {
    updateLatestNum();
  }

  // Get comic id from command or from (pseudo-)random number generator.
  const unsigned int num = determineComicId(command, message, mLatestNum);

  // XkcdData::get() only works when server can be reached.
  const auto info = XkcdData::get(num);
  if (!info.has_value() || info.value().img.empty())
  {
    return Message("Error: Could not get comic from xkcd.com!",
                   std::string("<strong>Error:</strong> Could not get comic from xkcd.com!"));
  }
  const XkcdData& data = info.value();
  const std::optional<std::string> mxcUri = uploadComic(data);

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
    xkcd.formatted_body = "xkcd.com # " + std::to_string(data.num) + ": <strong>"
            + htmlspecialchars(data.title)
            + "</strong><br />\n<br />\n<img src=\"" + mxcUri.value() + "\" alt=\"\"><br />\n"
            + "<br /><em>" + htmlspecialchars(data.alt) + "</em><br />Source: https://xkcd.com/"
            + std::to_string(data.num) + "/";
  }
  else
  {
    // Upload failed. Use original URL as fallback.
    xkcd.formatted_body = "xkcd.com # " + std::to_string(data.num) + ": <strong>"
            + htmlspecialchars(data.title) + "</strong><br />\n<br />\n"
            + data.img + "<br />\n<br /><em>" + htmlspecialchars(data.alt)
            + "</em><br />Source: https://xkcd.com/" + std::to_string(data.num)
            + "/";
  }
  return xkcd;
}

std::string Xkcd::helpOneLine(const std::string_view& command) const
{
  if (command == "xkcd")
  {
    return "displays a random comic from xkcd.com";
  }

  return std::string();
}

Message Xkcd::helpExtended(const std::string_view& command, const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "xkcd")
  {
    return Message("show a random comic from xkcd.com. If you want to get a "s
        + "particular comic, use its number after the command. For example, `"s
        .append(prefix) + "xkcd 1` will show the very first comic of xkcd.",
        "show a random comic from xkcd.com. If you want to get a "s
        + "particular comic, use its number after the command. For example, <code>"s
        .append(prefix) + "xkcd 1</code> will show the very first comic of xkcd.");
  }

  return Message();
}

} // namespace
