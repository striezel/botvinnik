/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2022  Dirk Stolle

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

#include "LibreTranslate.hpp"
#include <iostream>
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

std::vector<std::string> LibreTranslate::commands() const
{
  return { "tr", "tr-lang" };
}

Message LibreTranslate::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts)
{
  if ((command == "tr-lang") || (command == "tr-langs"))
  {
    return getLanguages();
  }

  if (command == "tr")
  {
    return getTranslation(command, message);
  }

  // unknown command
  return Message();
}

Message LibreTranslate::getLanguages()
{
  Curly curl;
  curl.setURL("https://libretranslate.com/languages");
  curl.addHeader("Accept: application/json");
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to libretranslate.com failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get a list of available languages failed. Server returned unexpected response.");
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from LibreTranslate server!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The request to get a list of available languages for translation failed. LibreTranslate server returned invalid JSON.");
  }
  if (!doc.is_array())
  {
    std::cerr << "Error while trying to parse JSON response from LibreTranslate server!" << std::endl
              << "Response is not an array. Response is: " << response << std::endl;
    return Message("The request to get a list of available languages for translation failed. LibreTranslate server returned an invalid JSON structure.");
  }
  Message result("Available languages and their corresponding language codes are:",
                 "Available languages and their corresponding language codes are:");
  for (const auto& item: doc)
  {
    simdjson::dom::element name;
    const auto error_name = item["name"].get(name);
    if (error_name || name.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error: JSON response from LibreTranslate does not contain 'name' element!" << std::endl;
      return Message("The request to get a list of available languages for translation failed. LibreTranslate server returned unexpected JSON format.");
    }
    simdjson::dom::element code;
    const auto error_code = item["code"].get(code);
    if (error_code || code.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error: JSON response from LibreTranslate does not contain 'code' element!" << std::endl;
      return Message("The request to get a list of available languages for translation failed. LibreTranslate server returned unexpected JSON format.");
    }
    const auto name_sv = name.get<std::string_view>().value();
    const auto code_sv = code.get<std::string_view>().value();
    result.body.append("\n").append(name_sv).append(": ").append(code_sv);
    result.formatted_body.append("<br />\n").append(name_sv).append(": <code>").append(code_sv).append("</code>");
  }
  return result;
}

Message LibreTranslate::getTranslation(const std::string_view& command, const std::string_view& message)
{
  std::string input(message.substr(command.size()));
  trim(input);
  if ((input.size() < 7) || (input[2] != ' ') || (input[5] != ' '))
  {
    return Message(std::string("Command ").append(command)
      .append(" must be followed by two language codes before the text to translate, e. g. `")
      .append(command).append(" en de Hello world!`."));
  }
  const auto source_language = toLowerString(input.substr(0, 2));
  const auto destination_language = toLowerString(input.substr(3, 2));
  if (source_language == destination_language)
  {
    return Message("Hint: Source language and destination language of a translation must be different.");
  }

  Curly curl;
  curl.addHeader("Accept: application/json");
  curl.setURL("https://libretranslate.com/translate");
  if (!curl.addPostField("source", source_language)
    || !curl.addPostField("target", destination_language)
    || !curl.addPostField("q", input.substr(6)))
  {
    std::cerr << "Error: Could not add POST fields to Curly request for "
              << "LibreTranslate!" << std::endl;
    return Message("Could not perform request to translation server!");
  }
  std::string response;
  if (!curl.perform(response))
  {
    std::cerr << "Error: Request to libretranslate.com failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get a translation failed. Server returned unexpected response.");
  }

  // Is the requested language supported?
  if ((curl.getResponseCode() == 400) && (response.find("not supported") != std::string::npos))
  {
    const std::string msg = "The requested language code is not supported. "
       + std::string("To get a list of available languages and their codes use")
       + " the command `tr-lang`.";
    return Message(msg);
  }

  if (curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to libretranslate.com failed!\n"
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get a translation failed. Server returned unexpected response.");
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from LibreTranslate server!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The request to get a translation failed. LibreTranslate server returned invalid JSON.");
  }

  simdjson::dom::element translated;
  const auto tr_error = doc["translatedText"].get(translated);
  if (tr_error || translated.type() != simdjson::dom::element_type::STRING)
  {
    std::cerr << "Error: JSON response from LibreTranslate does not contain 'translatedText' as string element!" << std::endl;
    return Message("The request to get a translation failed. LibreTranslate server returned unexpected JSON structure.");
  }

  Message result("The translation is:\n\n", "The translation is:<br />\n<br />\n");
  result.body.append(translated.get<std::string_view>().value());
  result.formatted_body.append("<blockquote>")
        .append(translated.get<std::string_view>().value())
        .append("</blockquote>");
  return result;
}

std::string LibreTranslate::helpOneLine(const std::string_view& command) const
{
  if (command == "tr")
  {
    return "translates a text from one language to another";
  }
  if (command == "tr-lang")
  {
    return "shows the available languages for translations";
  }

  return std::string();
}

} // namespace
