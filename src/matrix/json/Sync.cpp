/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2024  Dirk Stolle

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

#include "Sync.hpp"
#include <iostream>

namespace bvn
{

const int Sync::JsonError = 1; /**< non-zero error code for JSON errors */

bool Sync::parse(const std::string& json, std::string& nextBatch, std::vector<matrix::Room>& rooms, std::vector<std::string>& invitedRoomIds)
{
  rooms.clear();
  invitedRoomIds.clear();

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto parseError = parser.parse(json).get(doc);
  if (parseError)
  {
    std::cerr << "Error while syncing events: Unable to parse JSON data!\n"
              << "JSON is: " << json << '\n';
    return false;
  }
  simdjson::dom::element jsonNextBatch;
  const auto jsonError = doc["next_batch"].get(jsonNextBatch);
  if (jsonError || jsonNextBatch.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error while syncing events: JSON data does not contain"
              << " a next_batch element or it's not a string!\n";
    return false;
  }
  // nextBatch should only be overwritten, if it is not empty.
  if (!jsonNextBatch.get<std::string_view>().value().empty())
  {
    nextBatch = jsonNextBatch.get<std::string_view>().value();
  }
  else
  {
    std::cerr << "Error while syncing events: JSON data contains an empty "
              << "next_batch element, and that is against the specification!"
              << std::endl;
    return false;
  }

  // parse room data
  simdjson::dom::element jsonRooms;
  auto error = doc["rooms"].get(jsonRooms);
  if (error)
  {
    // No rooms element is available. This can happen, if no new room events
    // have occurred since the last sync request. In this case, nothing else is
    // left to do here.
    return true;
  }
  if (jsonRooms.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error: The rooms element is not an object!\n";
    return false;
  }

  {
    simdjson::dom::element join;
    jsonRooms["join"].tie(join, error);
    if (!error)
    {
      if (join.type() != simdjson::dom::element_type::OBJECT)
      {
        std::cerr << "Error: The join element is not an object!\n";
        return false;
      }

      // iterate over rooms
      simdjson::dom::object joinObject;
      join.get<simdjson::dom::object>().tie(joinObject, error);
      if (error)
      {
        std::cerr << "Error: 'join' is not an object!\n";
        return false;
      }

      if (parseJoinedRooms(joinObject, rooms) != 0)
      {
        return false;
      }
    }
  }

  {
    simdjson::dom::element invite;
    jsonRooms["invite"].tie(invite, error);
    if (!error)
    {
      if (invite.type() != simdjson::dom::element_type::OBJECT)
      {
        std::cerr << "Error: The invite element is not an object!\n";
        return false;
      }
      return parseInvitedRooms(invite, invitedRoomIds) == 0;
    }
  }

  // No errors occurred.
  return true;
}

int Sync::parseJoinedRooms(const simdjson::dom::object& join, std::vector<matrix::Room>& rooms)
{
  for (auto & keyValue : join)
  {
    matrix::Room room;
    room.id = keyValue.key;
    simdjson::dom::object roomObject;
    auto error = keyValue.value.get<simdjson::dom::object>(roomObject);
    if (error)
    {
      std::cerr << "Error: 'join' contains at least one non-object!\n";
      return JsonError;
    }

    simdjson::dom::element events;
    roomObject.at_pointer("/timeline/events").tie(events, error);
    if (error)
    {
      std::cerr << "Error: Could not find timeline/events pointer!\n";
      return JsonError;
    }
    if (events.type() != simdjson::dom::element_type::ARRAY)
    {
      std::cerr << "Error: events is not an array!\n";
      return JsonError;
    }

    for (const auto& elem : events)
    {
      simdjson::dom::element type;
      elem["type"].tie(type, error);
      if (error || type.type() != simdjson::dom::element_type::STRING)
      {
        std::cerr << "Error: Event type is missing or not a string!\n";
        return JsonError;
      }

      const auto typeString = type.get<std::string_view>().value();
      if (typeString == "m.room.message")
      {
        simdjson::dom::element msgtype;
        elem.at_pointer("/content/msgtype").tie(msgtype, error);
        if (error || msgtype.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-msgtype is missing or not a string!\n";
          return JsonError;
        }
        // Currently only text messages are relevant.
        if (msgtype.get<std::string_view>().value() == "m.text")
        {
          matrix::RoomMessageText txt;
          simdjson::dom::element data;
          // Body must always be present.
          elem.at_pointer("/content/body").tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::STRING)
          {
            std::cerr << "Error: content-body is missing or not a string!\n";
            return JsonError;
          }
          txt.body = data.get<std::string_view>().value();
          // format and formatted_body are optional.
          elem.at_pointer("/content/format").tie(data, error);
          if (!error && data.type() == simdjson::dom::element_type::STRING)
          {
            txt.format = data.get<std::string_view>().value();
          }
          elem.at_pointer("/content/formatted_body").tie(data, error);
          if (!error && data.type() == simdjson::dom::element_type::STRING)
          {
            txt.formatted_body = data.get<std::string_view>().value();
          }
          // sender should always be there.
          elem.at_pointer("/sender").tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::STRING)
          {
            std::cerr << "Error: Event's sender is missing or not a string!\n";
            return JsonError;
          }
          txt.sender = data.get<std::string_view>().value();
          // Timestamp should always be there.
          elem["origin_server_ts"].tie(data, error);
          if (error || data.type() != simdjson::dom::element_type::INT64)
          {
            std::cerr << "Error: Event's timestamp is missing or not an int64!\n";
            return JsonError;
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
        elem.at_pointer("/content/name").tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-name is missing or not a string!\n";
          return JsonError;
        }
        name.name = data.get<std::string_view>().value();
        // sender should always be there.
        elem["sender"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: Event's sender is missing or not a string!\n";
          return JsonError;
        }
        name.sender = data.get<std::string_view>().value();
        // Timestamp should always be there.
        elem["origin_server_ts"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::INT64)
        {
          std::cerr << "Error: Event's timestamp is missing or not an int64!\n";
          return JsonError;
        }
        name.server_ts = std::chrono::milliseconds(data.get<int64_t>().value());
        // Done.
        room.names.emplace_back(name);
      } // "m.room.name"
      else if (typeString == "m.room.topic")
      {
        matrix::RoomTopic topic;
        simdjson::dom::element data;
        // topic must always be present.
        elem.at_pointer("/content/topic").tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: content-topic is missing or not a string!\n";
          return JsonError;
        }
        topic.topic = data.get<std::string_view>().value();
        // sender should always be there.
        elem["sender"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::STRING)
        {
          std::cerr << "Error: Event's sender is missing or not a string!\n";
          return JsonError;
        }
        topic.sender = data.get<std::string_view>().value();
        // Timestamp should always be there.
        elem["origin_server_ts"].tie(data, error);
        if (error || data.type() != simdjson::dom::element_type::INT64)
        {
          std::cerr << "Error: Event's timestamp is missing or not an int64!\n";
          return JsonError;
        }
        topic.server_ts = std::chrono::milliseconds(data.get<int64_t>().value());
        // Done.
        room.topics.emplace_back(topic);
      } // "m.room.topic"
    } // for (events)

    rooms.emplace_back(room);
  } // for (Room)

  return 0;
}

int Sync::parseInvitedRooms(const simdjson::dom::element& invite, std::vector<std::string>& roomIds)
{
  roomIds.clear();
  // transform DOM element to object
  simdjson::dom::object inviteObject;
  simdjson::error_code error(simdjson::error_code::SUCCESS);
  invite.get<simdjson::dom::object>().tie(inviteObject, error);
  if (error)
  {
    std::cerr << "Error: 'invite' is not an object!\n";
    return JsonError;
  }

  // iterate over rooms
  for (auto & keyValue : inviteObject)
  {
    roomIds.emplace_back(keyValue.key);
  }

  return 0;
}

} // namespace
