/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#include "Wikipedia.hpp"
#include <iostream>
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"
#include "../../net/url_encode.hpp"

namespace bvn
{

Wikipedia::Wikipedia()
{
}

std::vector<std::string> Wikipedia::commands() const
{
  return { "wiki",
           "wikics", // Czech
           "wikicy", // Welsh
           "wikide", // German
           "wikiel", // Greek
           "wikien", // English
           "wikies", // Spanish
           "wikifr", // French
           "wikiit", // Italian
           "wikinl", // Dutch
           "wikipl", // Polish
           "wikipt", // Portuguese
           "wikiru", // Russian
           "wikitr", // Turkish
           "wikiuk", // Ukrainian
         };
}

Message Wikipedia::handleCommand(const std::string_view& command, const std::string_view& message,
                                 [[maybe_unused]] const std::string_view& userId,
                                 [[maybe_unused]] const std::string_view& roomId,
                                 [[maybe_unused]] const std::chrono::milliseconds& server_ts)
{
  if (command == "wiki" || command == "wikien")
  {
    return extract("en", command, message);
  }
  const auto all = commands();
  if (std::find(all.begin(), all.end(), std::string(command)) != all.end())
  {
    return extract(std::string(command.substr(4)), command, message);
  }

  // unknown command
  return Message();
}

std::string Wikipedia::helpOneLine(const std::string_view& command) const
{
  static const std::unordered_map<std::string, std::string> languages = {
      { "wikics", "Czech" },
      { "wikicy", "Welsh" },
      { "wikide", "German" },
      { "wikiel", "Greek" },
      { "wikien", "English" },
      { "wikies", "Spanish" },
      { "wikifr", "French" },
      { "wikiit", "Italian" },
      { "wikinl", "Dutch" },
      { "wikipl", "Polish" },
      { "wikipt", "Portuguese" },
      { "wikiru", "Russian" },
      { "wikitr", "Turkish" },
      { "wikiuk", "Ukrainian" }
  };
  if (command == "wiki" || command == "wikien")
  {
    return "gets extract from an article on the English Wikipedia";
  }
  const auto iter = languages.find(std::string(command));
  if (iter != languages.end())
  {
    return std::string("gets extract from an article on the ")
                       .append(iter->second).append(" Wikipedia");
  }

  // No help available.
  return std::string();
}

Message Wikipedia::extract(const std::string& lang, const std::string_view& command, const std::string_view& message) const
{
  // Is there a title after the command?
  if (message.size() <= command.size() + 1)
  {
    return Message(std::string("Error: There must be a keyword after the command ")
        .append(command).append(", e. g. '").append(command).append(" Einstein'."));
  }
  const std::string_view title = message.substr(command.size() + 1);
  std::string escapedTitle;
  try
  {
    escapedTitle = urlencode(title);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: URL-escaping threw an exception: " << ex.what() << std::endl;
    // Fallback: Return info about invalid chars.
    return Message("The request for Wikipedia article contains invalid characters in it's title.");
  }

  Curly curl;
  // URL is something like https://de.wikipedia.org/w/api.php?format=json&redirects=true&action=query&prop=extracts&exintro=true&exchars=1200&titles=Einstein.
  curl.setURL("https://" + lang + ".wikipedia.org/w/api.php?format=json&redirects=true&action=query&prop=extracts&exintro=true&exchars=1200&titles=" + escapedTitle);
  std::string response;
  if (!curl.perform(response))
  {
    std::cerr << "Error: Request to Mediawiki API failed!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get information from Wikipedia failed. Wikipedia server returned unexpected response.");
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from Wikipedia!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
  }
  simdjson::dom::element pages;
  const auto pagesError = doc.at_pointer("/query/pages").get(pages);
  if (pagesError || pages.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error while trying to parse JSON response from Wikipedia! JSON data does not contain a '/query/pages' object!" << std::endl;
    return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
  }

  simdjson::dom::object pagesObject;
  simdjson::error_code e;
  pages.get<simdjson::dom::object>().tie(pagesObject, e);
  if (e)
  {
    std::cerr << "Error while trying to parse JSON response from Wikipedia: 'pages' is not an object!" << std::endl;
    return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
  }

  for (auto & keyValue : pagesObject)
  {
    // If no page was found, the entry has key "-1".
    if (keyValue.key == "-1")
    {
      return Message(std::string("Wikipedia seems to have no page with the title \"")
             .append(title).append("\"."));
    }
    simdjson::dom::object page;
    keyValue.value.get<simdjson::dom::object>().tie(page, e);
    if (e)
    {
      std::cerr << "Error while trying to parse JSON response from Wikipedia: 'pages' element is not an object!" << std::endl;
      return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
    }
    simdjson::dom::element titleJson;
    page["title"].tie(titleJson, e);
    if (e || titleJson.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error while trying to parse JSON response from Wikipedia: 'title' element is missing or not a string!" << std::endl;
      return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
    }
    simdjson::dom::element jsonExtract;
    page["extract"].tie(jsonExtract, e);
    if (e || jsonExtract.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error while trying to parse JSON response from Wikipedia: 'extract' element is missing or not a string!" << std::endl;
      return Message("The request to get information from Wikipedia failed. Wikipedia server returned invalid JSON.");
    }

    std::string escapedApiTitle;
    try
    {
      escapedApiTitle = urlencode(titleJson.get<std::string_view>().value());
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: URL-escaping threw an exception: " << ex.what() << std::endl;
      // Fallback: Return info about invalid chars.
      return Message("The response from Wikipedia API contains invalid characters in it's title.");
    }

    return Message(
        // plain text
        std::string("Extract from Wikipedia:\n> ")
        .append(jsonExtract.get<std::string_view>().value())
        .append("\n\nSee https://").append(lang)
        .append(".wikipedia.org/wiki/").append(escapedApiTitle)
        .append(" for the full article."),
        // formatted HTML
        std::string("Extract from Wikipedia:<br />\n<blockquote>\n")
        .append(jsonExtract.get<std::string_view>().value())
        .append("</blockquote>\n<p>See <a href=\"https://").append(lang)
        .append(".wikipedia.org/wiki/").append(escapedApiTitle)
        .append("\">https://").append(lang).append(".wikipedia.org/wiki/")
        .append(escapedApiTitle).append("</a> for the full article.</p>"));
  } // for

  // Should not happen.
  return Message("The Wikipedia API found no page with the requested name or returned invalid JSON.");
}

} // namespace
