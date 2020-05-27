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
#include "../util/chrono.hpp"

namespace bvn
{

void parseSyncData(const simdjson::dom::element& doc, std::vector<matrix::Room>& rooms)
{
  rooms.clear();
  // parse room data
  auto [jsonRooms, error] = doc["rooms"];
  if (error || jsonRooms.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error: There is no rooms element or it's not an object!" << std::endl;
    return;
  }

  simdjson::dom::element join;
  jsonRooms["join"].tie(join, error);
  if (error || join.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error: There is no join element or it's not an object!" << std::endl;
    return;
  }

  // iterate over rooms
  simdjson::dom::object joinObject;
  join.get<simdjson::dom::object>().tie(joinObject, error);
  if (error)
  {
    std::cerr << "Error: 'join' is not an object!" << std::endl;
    return;
  }

  for (auto & keyValue : joinObject)
  {
    matrix::Room room;
    room.id = keyValue.key;
    simdjson::dom::object roomObject;
    keyValue.value.get<simdjson::dom::object>().tie(roomObject, error);
    if (error)
    {
      std::cerr << "Error: 'join' contains at least one non-object!" << std::endl;
      return;
    }

    simdjson::dom::element events;
    roomObject.at("timeline/events").tie(events, error);
    if (error)
    {
      std::cerr << "Error: Could not find timeline/events pointer!" << std::endl;
      return;
    }
    if (events.type() != simdjson::dom::element_type::ARRAY)
    {
      std::cerr << "Error: events is not an array!" << std::endl;
      return;
    }

    for (const auto& elem : events)
    {
      simdjson::dom::element type;
      elem["type"].tie(type, error);
      if (error || type.type() != simdjson::dom::element_type::STRING)
      {
        std::cerr << "Error: Event type is missing or not a string!" << std::endl;
        return;
      }

      const auto typeString = type.get<std::string_view>().value();
      if (typeString == "m.room.message")
      {
        simdjson::dom::element msgtype;
        elem.at("content/msgtype").tie(msgtype, error);
        if (error || msgtype.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-msgtype is missing or not a string!" << std::endl;
          return;
        }
        // Currently only text messages are relevant.
        if (msgtype.get<std::string_view>().value() == "m.text")
        {
          matrix::RoomMessageText txt;
          simdjson::dom::element data;
          // Body must always be present.
          elem.at("content/body").tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::STRING)
          {
            std::cerr << "Error: content-body is missing or not a string!" << std::endl;
            return;
          }
          txt.body = data.get<std::string_view>().value();
          // format and formatted_body are optional.
          elem.at("content/format").tie(data, error);
          if (!error && data.type() == simdjson::dom::element_type::STRING)
          {
            txt.format = data.get<std::string_view>().value();
          }
          elem.at("content/formatted_body").tie(data, error);
          if (!error && data.type() == simdjson::dom::element_type::STRING)
          {
            txt.formatted_body = data.get<std::string_view>().value();
          }
          // sender should always be there.
          elem.at("sender").tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::STRING)
          {
            std::cerr << "Error: Event's sender is missing or not a string!" << std::endl;
            return;
          }
          txt.sender = data.get<std::string_view>().value();
          // Timestamp should always be there.
          elem["origin_server_ts"].tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::INT64)
          {
            std::cerr << "Error: Event's timestamp is missing or not an int64!" << std::endl;
            return;
          }
          txt.server_ts = std::chrono::milliseconds(data.get<int64_t>().value());
          // Done.
          room.texts.emplace_back(txt);
        } // "m.text"
      } // "m.room.message"
      else if (typeString == "m.room.name")
      {
        matrix::RoomName name;
        simdjson::dom::element data;
        // name must always be present.
        elem.at("content/name").tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-name is missing or not a string!" << std::endl;
          return;
        }
        name.name = data.get<std::string_view>().value();
        // sender should always be there.
        elem["sender"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: Event's sender is missing or not a string!" << std::endl;
          return;
        }
        name.sender = data.get<std::string_view>().value();
        // Timestamp should always be there.
        elem["origin_server_ts"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::INT64)
        {
          std::cerr << "Error: Event's timestamp is missing or not an int64!" << std::endl;
          return;
        }
        name.server_ts = std::chrono::milliseconds(data.get<int64_t>().value());
        // Done.
        room.names.emplace_back(name);
      } // "m.room.name"
      else if (typeString == "m.room.topic")
      {
        matrix::RoomTopic topic;
        simdjson::dom::element data;
        // name must always be present.
        elem.at("content/topic").tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-topic is missing or not a string!" << std::endl;
          return;
        }
        topic.topic = data.get<std::string_view>().value();
        // sender should always be there.
        elem["sender"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: Event's sender is missing or not a string!" << std::endl;
          return;
        }
        topic.sender = data.get<std::string_view>().value();
        // Timestamp should always be there.
        elem["origin_server_ts"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::INT64)
        {
          std::cerr << "Error: Event's timestamp is missing or not an int64!" << std::endl;
          return;
        }
        topic.server_ts = std::chrono::milliseconds(data.get<int64_t>().value());
        // Done.
        room.topics.emplace_back(topic);
      } // "m.room.topic"
    } // for (events)

    rooms.emplace_back(room);
  } // for (Room)
}

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
    roomIds.push_back(std::string(elem.get<std::string_view>().value()));
  }

  return true;
}

std::string Matrix::encodeRoomId(const std::string& roomId)
{
  std::string encodedRoomId = roomId;
  // TODO: use better URL encoding
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

bool Matrix::sync(std::string& events, std::string& nextBatch, std::vector<matrix::Room>& rooms, const std::string& since)
{
  if (!isLoggedIn())
  {
    std::cerr << "Error: Need to be logged in to sync events!" << std::endl;
    return false;
  }

  std::string response;

  Curly curl;
  if (!since.empty())
    // incremental sync
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/sync?since=" + since);
  else
    // initial sync
    curl.setURL(conf.homeServer() + "/_matrix/client/r0/sync");

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

  parseSyncData(doc, rooms);

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

} // namespace
