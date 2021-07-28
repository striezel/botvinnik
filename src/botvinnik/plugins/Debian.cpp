/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#include "Debian.hpp"
#include "../../../third-party/simdjson/simdjson.h"
#include "../../net/Curly.hpp"
#include "../../net/htmlspecialchars.hpp"
#include "../../net/url_encode.hpp"
#include "../../util/Strings.hpp"

namespace bvn
{

Debian::Debian()
{
}

std::vector<std::string> Debian::commands() const
{
  return { "deb", "deb11", "deb10", "deb9", "deb8" };
}

void Debian::getVersion(Packages::nameVersion& pack, const std::string& suite)
{
  std::string encodedPackageName;
  try
  {
    encodedPackageName = urlencode(pack.first);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not URL-encode package name!" << std::endl;
    return;
  }

  Curly curl;
  // URL is something like https://sources.debian.org/api/src/mc/?suite=buster, e. g. for package "mc".
  curl.setURL("https://sources.debian.org/api/src/" + encodedPackageName + "/?suite=" + suite);
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to Debian package API (versions) failed!" << std::endl
              << "URL: " << curl.getURL() << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return;
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from Debian!" << std::endl
              << "Response is: " << response << std::endl;
    return;
  }
  simdjson::dom::element v;
  const auto vError = doc.at_pointer("/versions/0/version").get(v);
  if (vError || v.type() != simdjson::dom::element_type::STRING)
  {
    return;
  }
  pack.second = v.get<std::string_view>().value();
}

void Debian::getVersions(Packages& packs, const std::string& suite)
{
  if (!packs.exact.first.empty())
    getVersion(packs.exact, suite);

  const std::size_t limit = std::min(static_cast<std::size_t>(3), packs.others.size());
  for (std::size_t i = 0; i < limit; ++i)
  {
    getVersion(packs.others[i], suite);
  }
}

Message Debian::packageSearch(const std::string_view& command, const std::string_view& message, const std::string& suite)
{
  std::string packageName(message.substr(command.size() + 1));
  trim(packageName);
  if (packageName.size() < 2)
  {
    return Message("Debian package name to search for must be at least two characters long!");
  }

  // Debian packages always use lower case letters, so transform any upper case
  // characters to lower case.
  packageName = toLowerString(packageName);

  std::string encodedPackageName;
  try
  {
    encodedPackageName = urlencode(packageName);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not URL-encode package name!" << std::endl;
    return Message("Error: Could not get information for package '" + packageName + "'!");
  }

  Curly curl;
  // URL is something like https://sources.debian.org/api/search/mc/?suite=stretch, e. g. for package "mc".
  curl.setURL(std::string("https://sources.debian.org/api/search/")
              .append(encodedPackageName).append("/?suite=").append(suite));
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Request to Debian package search API failed!" << std::endl
              << "URL: " << curl.getURL() << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return Message("The request to get information about Debian packages failed. Debian server returned unexpected response.");
  }

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(response).get(doc);
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from Debian!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The request to get information about Debian packages failed. Debian server returned invalid JSON.");
  }

  Packages packs;
  {
    simdjson::dom::element exact;
    const auto exactError = doc.at_pointer("/results/exact").get(exact);
    if (exactError)
    {
      std::cerr << "Error: JSON response from Debian does not contain '/results/exact' element!" << std::endl
                << "Response is: " << response << std::endl;
      return Message("The request to get information about Debian packages failed. Debian server returned unexpected JSON format.");
    }
    if (exact.type() == simdjson::dom::element_type::OBJECT)
    {
      simdjson::dom::element exactName;
      const auto error2 = exact["name"].get(exactName);
      if (error2 || exactName.type() != simdjson::dom::element_type::STRING)
      {
        std::cerr << "Error: JSON response from Debian does not contain '/results/exact/name' element!" << std::endl
                  << "Response is: " << response << std::endl;
        return Message("The request to get information about Debian packages failed. Debian server returned unexpected JSON format.");
      }
      packs.exact = std::pair(exactName.get<std::string_view>().value(), "");
    }
  }

  simdjson::dom::element other;
  const auto otherError = doc.at_pointer("/results/other").get(other);
  if (otherError || other.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error: JSON response from Debian does not contain '/results/other' element!" << std::endl
              << "Response is: " << response << std::endl;
    return Message("The request to get information about Debian packages failed. Debian server returned unexpected JSON format.");
  }

  for (const auto & item : other)
  {
    simdjson::dom::element itemName;
    const auto itemError = item["name"].get(itemName);
    if (itemError || itemName.type() != simdjson::dom::element_type::STRING)
    {
      std::cerr << "Error: JSON response from Debian does not contain '/results/other/name' element!" << std::endl
                << "Response is: " << response << std::endl;
      return Message("The request to get information about Debian packages failed. Debian server returned unexpected JSON format.");
    }
    packs.others.emplace_back(std::pair(itemName.get<std::string_view>().value(), std::string()));
  }

  getVersions(packs, suite);

  return formatResult(packs, suite, packageName);
}

Message Debian::formatResult(const Packages& packs, const std::string& suite, const std::string_view& packageName)
{
  if (packs.exact.first.empty() && packs.others.empty())
  {
    return Message(std::string("Could not find a matching package for '")
                   .append(packageName).append("' in Debian ").append(suite)
                   .append("."));
  }

  Message result(std::string("Result for package '").append(packageName)
                 .append("' in Debian ").append(suite).append("\n\n"),
                 std::string("Result for package '<strong>").append(htmlspecialchars(packageName))
                 .append("</strong>' in Debian ").append(suite).append("<br >\n<br >\n"));
  if (!packs.exact.first.empty())
  {
    result.body.append("Exact match: ").append(packs.exact.first);
    result.formatted_body.append("Exact match: <a href=\"https://packages.debian.org/source/")
                         .append(suite).append("/").append(packs.exact.first)
                         .append("\"><strong>").append(packs.exact.first).append("</strong></a>");
    if (!packs.exact.second.empty())
    {
      result.body.append(", version ").append(packs.exact.second);
      result.formatted_body.append(", version ").append(htmlspecialchars(packs.exact.second));
    }
    result.body.append("\n");
    result.formatted_body.append("<br>\n");
  }
  else
  {
    // no exact match
    result.body.append("Exact match: none\n");
    result.formatted_body.append("Exact match: none<br />\n");
  }
  if (!packs.others.empty())
  {
    result.body.append("Other, partial matches:");
    result.formatted_body.append("Other, partial matches:<br />\n<ul>");
    // Some search terms (e.g. "python") yield more than 1000 results, so the
    // size of the result must be limited. Otherwise the response cannot be sent
    // to the Matrix server, because the specification limits the maximum size
    // of events to "65 KB". This includes plain text body, formatted body, and
    // some other metadata.
    const std::size_t otherLimit = std::min(static_cast<std::size_t>(100), packs.others.size());
    for (std::size_t i = 0; i < otherLimit; ++i)
    {
      const auto& nv = packs.others.at(i);
      result.body.append("\n* ").append(nv.first);
      result.formatted_body.append("\n  <li><a href=\"https://packages.debian.org/source/")
                           .append(suite).append("/").append(nv.first)
                           .append("\"><strong>").append(nv.first).append("</strong></a>");
      if (!nv.second.empty())
      {
        result.body.append(", version ").append(nv.second);
        result.formatted_body.append(", version ").append(htmlspecialchars(nv.second));
      }
      result.formatted_body.append("</li>");
    }
    result.formatted_body.append("\n</ul>");
    if (packs.others.size() > otherLimit)
    {
      result.body.append("\n\nHint: The search returned ")
                 .append(std::to_string(packs.others.size()))
                 .append(" partial matches, but only the first ")
                 .append(std::to_string(otherLimit))
                 .append(" of them are shown.");
      result.formatted_body.append("<br />\n<br />\n<em>Hint: The search returned ")
                 .append(std::to_string(packs.others.size()))
                 .append(" partial matches, but only the first ")
                 .append(std::to_string(otherLimit))
                 .append(" of them are shown.</em>");
    }
  }
  else
  {
    result.body.append("Other, partial matches: none");
    result.formatted_body.append("Other, partial matches: none");
  }
  return result;
}

Message Debian::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::string_view& roomId, const std::chrono::milliseconds& server_ts)
{
  if (message.size() <= command.size() + 1)
  {
    return Message(std::string(userId)
        .append(": You have to give a package name (or part of a package name) after the '")
        .append(command).append("' command."));
  }

  if (command == "deb" || command == "deb10")
  {
    return packageSearch(command, message, "buster");
  }
  if (command == "deb11")
  {
    return packageSearch(command, message, "bullseye");
  }
  if (command == "deb9")
  {
    return packageSearch(command, message, "stretch");
  }
  if (command == "deb8")
  {
    return packageSearch(command, message, "jessie");
  }

  // unknown command
  return Message();
}

std::string Debian::helpOneLine(const std::string_view& command) const
{
  if (command == "deb")
  {
    return "alias for deb10";
  }
  if (command == "deb11")
  {
    return "gets a list of available packages in Debian 11 (a.k.a \"bullseye\"), the future stable release";
  }
  if (command == "deb10")
  {
    return "gets a list of available packages in Debian 10 (a.k.a \"buster\"), the current stable release";
  }
  if (command == "deb9")
  {
    return "gets a list of available packages in Debian 9 (a.k.a \"stretch\"), the old stable release";
  }
  if (command == "deb8")
  {
    return "gets a list of available packages in Debian 8 (a.k.a \"jessie\")";
  }

  return std::string();
}

} // namespace
