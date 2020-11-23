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
#include "../net/url_encode.hpp"
#include "../util/chrono.hpp"
#include "../util/Strings.hpp"
#include "json/PowerLevels.hpp"
#include "json/Sync.hpp"

namespace bvn
{

Matrix::Matrix(const Configuration& _conf)
: conf(_conf),
  accessToken(std::string()),
  transactionId(0)
{
}

Matrix::~Matrix()
{
  if (isLoggedIn())
    logout();
}

const Configuration& Matrix::configuration() const
{
  return conf;
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
      { "initial_device_display_name", "botvinnik, " + bvn::version + "(started at " + nowToString() + ")" }
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
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/logout");
  curl.addHeader("Authorization: Bearer " + accessToken);
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

bool Matrix::joinedRooms(std::vector<std::string>& roomIds)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to get current rooms!" << std::endl;
    return false;
  }

  std::string response;

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/joined_rooms");
  curl.addHeader("Authorization: Bearer " + accessToken);
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Listing joined rooms failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error while trying to list rooms: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  const auto [joined_rooms, jsonError] = doc["joined_rooms"];
  if (jsonError || joined_rooms.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error while trying to list rooms: JSON data does not contain"
              << " a joined_rooms element or it's not an array!" << std::endl;
    return false;
  }

  roomIds.clear();
  for (const auto elem : joined_rooms)
  {
    if (elem.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error: Returned room id is not a string!" << std::endl;
      return false;
    }
    roomIds.emplace_back(std::string(elem.get<std::string_view>().value()));
  }

  return true;
}

std::string Matrix::encodeRoomId(const std::string& roomId)
{
  try
  {
    return urlencode(roomId);
  }
  catch(const std::exception& ex)
  {
    std::cerr << "Error: URL-encoding of room id failed!\n"
              << ex.what() << std::endl
              << "Falling back to simpler encoding algorithm." << std::endl;
  }

  std::string encodedRoomId = roomId;
  // Note: This is not perfectly safe, but it works for valid room ids.
  auto pos = encodedRoomId.find(':');
  if (pos != std::string::npos)
    encodedRoomId.replace(pos, 1, "%3A");
  pos = encodedRoomId.find('!');
  if (pos != std::string::npos)
    encodedRoomId.replace(pos, 1, "%21");

  return encodedRoomId;
}

bool Matrix::roomName(const std::string& roomId, std::string& name)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to get room name!" << std::endl;
    return false;
  }

  std::string response;

  Curly curl;
  const std::string encodedRoomId = encodeRoomId(roomId);
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/state/m.room.name/");
  curl.addHeader("Authorization: Bearer " + accessToken);
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Listing joined rooms failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error while trying get room name: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  const auto [jsonName, jsonError] = doc["name"];
  if (jsonError || jsonName.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to get room name: JSON data does not contain"
              << " a name element or it's not a string!" << std::endl;
    return false;
  }

  name = jsonName.get<std::string_view>().value();
  return true;
}

bool Matrix::joinRoom(const std::string& roomId)
{
  return roomMembershipChange(roomId, "join");
}

bool Matrix::leaveRoom(const std::string& roomId)
{
  return roomMembershipChange(roomId, "leave");
}

bool Matrix::forgetRoom(const std::string& roomId)
{
  return roomMembershipChange(roomId, "forget");
}

bool Matrix::roomMembershipChange(const std::string& roomId, const std::string& change)
{
  if (change != "join" && change != "leave" && change != "forget")
  {
    std::cerr << "Error: Room membership action must be one of  must be one of"
              << " 'join', 'leave' or 'forget'!" << std::endl;
    return false;
  }
  if (roomId.empty())
  {
    std::cerr << "Error: Id of the room to " << change << " must not be empty!" << std::endl;
    return false;
  }
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to " << change << " a room!" << std::endl;
    return false;
  }

  std::string encodedRoomId;
  try
  {
    encodedRoomId = urlencode(roomId);
  }
  catch(const std::exception& ex)
  {
    std::cerr << "Error: URL-encoding of room id failed!"
              << std::endl << ex.what() << std::endl;
    return false;
  }

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/" + change);
  curl.addHeader("Authorization: Bearer " + accessToken);
  curl.setPostBody("");
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Failed to " << change << " room '" << roomId << "'!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  return true;
}

std::optional<matrix::PowerLevels> Matrix::powerLevels(const std::string& roomId)
{
  if (roomId.empty())
  {
    std::cerr << "Error: Room id for power level retrieval must not be empty!" << std::endl;
    return std::optional<matrix::PowerLevels>();
  }
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to get room power levels!" << std::endl;
    return std::optional<matrix::PowerLevels>();
  }
  std::string encodedRoomId;
  try
  {
    encodedRoomId = urlencode(roomId);
  }
  catch(const std::exception& ex)
  {
    std::cerr << "Error: URL-encoding of room id failed!"
              << std::endl << ex.what() << std::endl;
    return std::optional<matrix::PowerLevels>();
  }

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/state/m.room.power_levels");
  curl.addHeader("Authorization: Bearer " + accessToken);
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Failed to get power levels of room '" << roomId << "'!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<matrix::PowerLevels>();
  }

  return matrix::json::parsePowerLevels(response);
}

bool Matrix::sync(std::string& events, std::string& nextBatch, std::vector<matrix::Room>& rooms, std::vector<std::string>& invites, const std::string& since)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to sync events!" << std::endl;
    return false;
  }

  std::string response;

  Curly curl;
  if (!since.empty())
  {
    // incremental sync
    std::string encodedSince;
    try
    {
      encodedSince = urlencode(since);
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: URL-encoding of parameter 'since' failed!"
                << std::endl << ex.what() << std::endl;
      return false;
    }
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/sync?since=" + encodedSince);
  }
  else
  {
    // initial sync
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/sync");
  }

  curl.addHeader("Authorization: Bearer " + accessToken);
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Syncing events failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error while syncing events: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  const auto [jsonNextBatch, jsonError] = doc["next_batch"];
  if (jsonError || jsonNextBatch.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while syncing events: JSON data does not contain"
              << " a next_batch element or it's not a string!" << std::endl;
    return false;
  }
  nextBatch = jsonNextBatch.get<std::string_view>().value();
  events = response;

  Sync::parse(doc, rooms, invites);

  return true;
}

bool Matrix::sendMessage(const std::string& roomId, const Message& message)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to send messages!" << std::endl;
    return false;
  }

  if (message.body.empty())
  {
    std::cerr << "Error: Sending empty messages is useless!" << std::endl;
    return false;
  }

  nlohmann::json body = {
      { "msgtype", "m.text" },
      { "body", message.body }
  };
  if (!message.formatted_body.empty())
  {
    body["formatted_body"] = message.formatted_body;
    body["format"] = "org.matrix.custom.html";
  }

  std::string response;
  Curly curl;
  // PUT /_matrix/client/r0/rooms/{roomId}/send/{eventType}/{txnId}
  const std::string encodedRoomId(encodeRoomId(roomId));
  const uint_least32_t txnId = transactionId.fetch_add(1);
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/send/m.room.message/" + std::to_string(txnId));
  curl.addHeader("Authorization: Bearer " + accessToken);
  curl.addHeader("Content-Type: application/json");
  curl.setPutData(body.dump());
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Sending text message failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  return true;
}

std::optional<int64_t> Matrix::getUploadLimit()
{
  if (!isLoggedIn())
    return std::optional<int64_t>();

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/media/r0/config");
  curl.addHeader("Authorization: Bearer " + accessToken);
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Could not retrieve upload limit!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<int64_t>();
  }

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error retrieving upload limit: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<int64_t>();
  }
  const auto [uploadLimit, jsonError] = doc["m.upload.size"];
  if (jsonError || uploadLimit.type() != simdjson::dom::element_type::INT64)
  {
    std::clog << "Warning: Server did not disclose upload size!" << std::endl;
    return std::optional<int64_t>(-1);
  }

  return uploadLimit.get<int64_t>().value();
}

std::optional<std::string> Matrix::uploadString(const std::string& data, const std::string& contentType, const std::string& fileName)
{
  if (!isLoggedIn())
    return std::optional<std::string>();

  std::string encodedFileName;
  try
  {
    encodedFileName = urlencode(fileName);
  }
  catch(const std::exception& ex)
  {
    std::cerr << "Error: URL-encoding of file name for upload failed!"
              << std::endl << ex.what() << std::endl;
    return std::optional<std::string>();
  }

  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/media/r0/upload?filename=" + encodedFileName);
  curl.addHeader("Authorization: Bearer " + accessToken);
  curl.addHeader("Content-Type: " + contentType);
  if (!curl.setPostBody(data))
  {
    std::cerr << "Error: Could not set body for POST request of file upload."
              << std::endl;
    return std::optional<std::string>();
  }
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Could not upload file data to content repository!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response);
  if (error)
  {
    std::cerr << "Error upload file: Unable to parse JSON response!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<std::string>();
  }
  const auto [contentUri, jsonError] = doc["content_uri"];
  if (jsonError || contentUri.type() != simdjson::dom::element_type::STRING)
  {
    std::clog << "Warning: Server did not return a content URI for upload!" << std::endl;
    return std::optional<std::string>();
  }

  return std::string(contentUri.get<std::string_view>().value());
}

std::optional<std::string> Matrix::uploadImage(const std::string& imgUrl)
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
  return uploadString(imageData, contentType, fileName);
}

} // namespace
