/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2023  Dirk Stolle

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

#ifdef BVN_USER_AGENT
void addUserAgent(Curly& curl)
{
  curl.addHeader(std::string("User-Agent: ") + bvn::userAgent);
}
#endif


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
      { "initial_device_display_name", "botvinnik, " + bvn::version + " (started at " + nowToString() + ")" }
    };
    Curly curl;
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/login");
    curl.addHeader("Content-Type: application/json");
    #ifdef BVN_USER_AGENT
    addUserAgent(curl);
    #endif
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
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to log in: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  simdjson::dom::element token;
  const auto tokenError = doc["access_token"].get(token);
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Listing joined rooms failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to list rooms: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return false;
  }
  simdjson::dom::element joined_rooms;
  const auto jsonError = doc["joined_rooms"].get(joined_rooms);
  if (jsonError || !joined_rooms.is_array())
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

std::optional<std::string>  Matrix::roomName(const std::string& roomId)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to get room name!" << std::endl;
    return std::optional<std::string>();
  }

  std::string response;

  Curly curl;
  const std::string encodedRoomId = encodeRoomId(roomId);
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/state/m.room.name/");
  curl.addHeader("Authorization: Bearer " + accessToken);
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    if (curl.getResponseCode() == 404)
    {
      // Not found means that no name has been set.
      return std::string();
    }
    std::cerr << "Error: Listing joined rooms failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying get room name: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<std::string>();
  }
  simdjson::dom::element jsonName;
  const auto jsonError = doc["name"].get(jsonName);
  if (jsonError || jsonName.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while trying to get room name: JSON data does not contain"
              << " a name element or it's not a string!" << std::endl;
    return std::optional<std::string>();
  }

  return std::string(jsonName.get<std::string_view>().value());
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
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

bool Matrix::sync(std::string& nextBatch, std::vector<matrix::Room>& rooms, std::vector<std::string>& invites, const std::string& since)
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Syncing events failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return false;
  }

  return Sync::parse(response, nextBatch, rooms, invites);
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
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

bool Matrix::sendImage(const std::string& roomId, const std::string& mxcUri, const std::string mimeType)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to send images!" << std::endl;
    return false;
  }

  if (mxcUri.empty())
  {
    std::cerr << "Error: Sending empty MXC URIs is useless!" << std::endl;
    return false;
  }

  if (mxcUri.find("mxc://") != 0)
  {
    std::cerr << "Error: Given image URL is not an MXC URI!" << std::endl;
    return false;
  }

  nlohmann::json message = {
      { "msgtype", "m.image" },
      { "body", "[This is an image.]" },
      { "url", mxcUri }
  };

  if (!mimeType.empty())
  {
    message["info"]["mimetype"] = mimeType;
  }

  std::string response;
  Curly curl;
  // PUT /_matrix/client/r0/rooms/{roomId}/send/{eventType}/{txnId}
  const std::string encodedRoomId(encodeRoomId(roomId));
  const uint_least32_t txnId = transactionId.fetch_add(1);
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId + "/send/m.room.message/" + std::to_string(txnId));
  curl.addHeader("Authorization: Bearer " + accessToken);
  curl.addHeader("Content-Type: application/json");
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  curl.setPutData(message.dump());
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Sending image message failed!" << std::endl
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Could not retrieve upload limit!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<int64_t>();
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error retrieving upload limit: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<int64_t>();
  }
  simdjson::dom::element uploadLimit;
  const auto jsonError = doc["m.upload.size"].get(uploadLimit);
  if (jsonError || uploadLimit.type() != simdjson::dom::element_type::INT64)
  {
    std::clog << "Warning: Server did not disclose upload size!" << std::endl;
    return std::optional<int64_t>(-1);
  }

  return uploadLimit.get<int64_t>().value();
}

std::optional<std::string> Matrix::getSynapseVersion()
{
  Curly curl;
  curl.setURL(conf.homeServer() + "/_synapse/admin/v1/server_version");
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    return std::nullopt;
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error retrieving server version: Unable to parse JSON data!" << std::endl
              << "Response is: " << response << std::endl;
    return std::nullopt;
  }
  simdjson::dom::element server_version;
  const auto jsonError = doc["server_version"].get(server_version);
  if (jsonError || server_version.type() != simdjson::dom::element_type::STRING)
  {
    std::clog << "Warning: Server did not disclose its version!" << std::endl;
    return std::nullopt;
  }

  return std::string(server_version.get<std::string_view>().value());
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
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
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
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error upload file: Unable to parse JSON response!" << std::endl
              << "Response is: " << response << std::endl;
    return std::optional<std::string>();
  }
  simdjson::dom::element contentUri;
  const auto jsonError = doc["content_uri"].get(contentUri);
  if (jsonError || contentUri.type() != simdjson::dom::element_type::STRING)
  {
    std::clog << "Warning: Server did not return a content URI for upload!" << std::endl;
    return std::optional<std::string>();
  }

  return std::string(contentUri.get<std::string_view>().value());
}

std::string extract_file_name_from_url(const std::string& url)
{
  std::string fileName;
  std::string::size_type pos = url.rfind('/');
  if ((pos != std::string::npos) && (url.size() > pos + 1))
    fileName = url.substr(pos + 1);
  else
    fileName = "image.data";

  pos = fileName.rfind('?');
  if ((pos != std::string::npos) && (pos > 3))
    fileName.erase(pos);

  return fileName;
}

std::optional<std::string> Matrix::uploadImage(const std::string& imgUrl)
{
  std::string imageData;
  {
    Curly curl;
    curl.setURL(imgUrl);
    #ifdef BVN_USER_AGENT
    addUserAgent(curl);
    #endif
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
  else if (endsWith(imgUrl, ".gif") || (imgUrl.find(".gif") != std::string::npos))
    contentType = "image/gif";

  const std::string fileName = extract_file_name_from_url(imgUrl);

  std::clog << "Info: Uploading image " << imgUrl << " ("
            << static_cast<long int>(imageData.size())
            << " bytes) to Matrix ..." << std::endl;
  return uploadString(imageData, contentType, fileName);
}

std::optional<std::string> Matrix::encryptionAlgorithm(const std::string& roomId)
{
  if (!isLoggedIn())
    return std::optional<std::string>();

  const auto encodedRoomId = encodeRoomId(roomId);
  Curly curl;
  curl.setURL(conf.homeServer() + "/_matrix/client/r0/rooms/" + encodedRoomId
                                + "/state/m.room.encryption");
  curl.addHeader("Authorization: Bearer " + accessToken);
  #ifdef BVN_USER_AGENT
  addUserAgent(curl);
  #endif
  std::string response;
  if (!curl.perform(response))
  {
    std::cerr << "Error: Could not get room encryption algorithm!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  // 404 Not Found means that the room has no encryption.
  if (curl.getResponseCode() == 404)
  {
    // Return an empty string to signal unencrypted room.
    return std::optional<std::string>("");
  }

  if (curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Could not get room encryption algorithm!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error getting room's encryption algorithm: Unable to parse "
              << "JSON response!" << std::endl << "Response is: " << response
              << std::endl;
    return std::optional<std::string>();
  }
  simdjson::dom::element algorithm;
  const auto jsonError = doc["algorithm"].get(algorithm);
  if (jsonError || algorithm.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error: Server did not respond with encryption algorithm!"
              << std::endl;
    return std::optional<std::string>();
  }

  return std::optional<std::string>(algorithm.get<std::string_view>().value());
}

} // namespace
