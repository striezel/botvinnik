/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2021  Dirk Stolle

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

#include "disease.sh.hpp"
#include <regex>
#include <unordered_map>
#include "../../../net/Curly.hpp"
#include "../../../../third-party/simdjson/simdjson.h"

namespace bvn
{

namespace disease_sh
{

/**
 * Constructs the URL for a request to the historical API of disease.sh.
 *
 * @param  geoId  geo id (i. e. two letter country code) of a country
 * @param  province  name of the province as seen in the API response for the
           country; if empty, the numbers for the whole country are requested
 * @param  all   whether to collect recent or all data
 * @return Returns a string containing the URL.
 */
std::string constructHistoricalApiUrl(const std::string& geoId, const std::string& province, const bool all)
{
  if (province.empty())
  {
    if (all)
    {
      return std::string("https://corona.lmao.ninja/v3/covid-19/historical/")
            .append(geoId).append("?lastdays=all");
    }
    else
    {
      return std::string("https://corona.lmao.ninja/v3/covid-19/historical/")
            .append(geoId);
    }
  }
  else
  {
    if (all)
    {
      return std::string("https://corona.lmao.ninja/v3/covid-19/historical/")
            .append(geoId).append("/").append(province).append("?lastdays=all");
    }
    else
    {
      return std::string("https://corona.lmao.ninja/v3/covid-19/historical/")
            .append(geoId).append("/").append(province);
    }
  }
}

/**
 * Constructs the URL for a request to the historical API of disease.sh for an county of the USA.
 *
 * @param  county  name of the county as seen in the API
 * @param  all     whether to collect recent or all data
 * @return Returns a string containing the URL.
 */
std::string constructHistoricalApiUrlUsaCounties(const std::string& county, const bool all)
{
  if (all)
    return std::string("https://corona.lmao.ninja/v3/covid-19/historical/usacounties/")
          .append(county).append("?lastdays=all");
  else
    return std::string("https://corona.lmao.ninja/v3/covid-19/historical/usacounties/")
          .append(county);
}

/**
 * Performs a request to the API of disease.sh for a given URL and deserializes the JSON.
 *
 * @param  url   URL of the endpoint
 * @return Returns a string containing the API response in case of success.
 *         If an error occurred, an empty optional is returned.
 */
std::optional<std::string> performApiRequest(const std::string& url)
{
  Curly curl;
  curl.setURL(url);
  std::string response;
  if (!curl.perform(response) || curl.getResponseCode() != 200)
  {
    std::cerr << "Error: Failed to get COVID-19 case numbers from disease.sh API!" << std::endl
              << "HTTP status code: " << curl.getResponseCode() << std::endl
              << "Response: " << response << std::endl;
    return std::optional<std::string>();
  }

  return response;
}

/**
 * Parses JSON from historical API of disease.sh.
 *
 * @param  json  the parsed JSON element from simdjson
 * @return Returns CovidNumbers containing the new daily cases.
 *         If an error occurred, an empty CovidNumbers is returned.
 */
CovidNumbers parseJsonTimeline(const simdjson::dom::element& json)
{
  const auto [cases_elem, error] = json.at_pointer("/timeline/cases");
  if (error)
  {
    std::cerr << "JSON from API does not contain cases in timeline!" << std::endl;
    return CovidNumbers();
  }
  const auto [cases, err] = cases_elem.get_object();
  if (err)
  {
    std::cerr << "JSON from API contains cases, but it is not an object!" << std::endl;
    return CovidNumbers();
  }

  const auto [deaths_elem, error_d] = json.at_pointer("/timeline/deaths");
  if (error_d)
  {
    std::cerr << "JSON from API does not contain deaths in timeline!" << std::endl;
    return CovidNumbers();
  }
  const auto [deaths, err2] = deaths_elem.get_object();
  if (err2)
  {
    std::cerr << "JSON from API contains deaths, but it is not an object!" << std::endl;
    return CovidNumbers();
  }

  // Date is something like e. g. "12/31/20" for 31st December 2020 or
  // "1/1/21" for 1st January 2021.
  const std::regex date_exp("^([0-9]+)/([0-9]+)/([0-9]{2})$");
  std::unordered_map<std::string, CovidNumbersElem> numbers;

  for (auto& keyValue : cases)
  {
    const std::string key(keyValue.key);
    std::smatch match;
    if (!std::regex_match(key, match, date_exp))
    {
      std::cout << "Found date '" << key << "' which does not match the pattern. Skipping it." << std::endl;
      continue;
    }
    const std::string dayStr = match[2];
    const std::string monthStr = match[1];
    const auto yearStr = match[3];
    const auto isoDate = std::string("20").append(yearStr) + "-"
                       + std::string(monthStr.size() == 1, '0') + monthStr + "-"
                       + std::string(dayStr.size() == 1, '0') + dayStr;
    const auto [infections, error_i64] = keyValue.value.get<int64_t>();
    if (error_i64)
    {
      std::cerr << "Error: JSON from API contains a non-number as number of infections!" << std::endl;
      return CovidNumbers();
    }
    CovidNumbersElem elem;
    elem.cases = infections;
    elem.deaths = std::numeric_limits<int64_t>::min();
    elem.incidence14 = std::numeric_limits<double>::quiet_NaN();
    elem.date = isoDate;
    numbers[isoDate] = elem;
  }
  for (auto& keyValue : deaths)
  {
    const std::string key(keyValue.key);
    std::smatch match;
    if (!std::regex_match(key, match, date_exp))
    {
      std::cout << "Found date '" << key << "' which does not match the pattern. Skipping it." << std::endl;
      continue;
    }
    const std::string dayStr = match[2];
    const std::string monthStr = match[1];
    const auto yearStr = match[3];
    const auto isoDate = std::string("20").append(yearStr) + "-"
                       + std::string(monthStr.size() == 1, '0') + monthStr + "-"
                       + std::string(dayStr.size() == 1, '0') + dayStr;
    const auto [deaths, error_i64] = keyValue.value.get<int64_t>();
    if (error_i64)
    {
      std::cerr << "Error: JSON from API contains a non-number as number of deaths!" << std::endl;
      return CovidNumbers();
    }
    auto iter = numbers.find(isoDate);
    if (iter == numbers.end())
    {
      std::cerr << "Date '" << isoDate << "' is not present in both timelines!" << std::endl;
      return CovidNumbers();
    }
    iter->second.deaths = deaths;
  }

  // Get it out of the map ...
  std::vector<CovidNumbersElem> elems;
  elems.reserve(numbers.size());
  for (auto kv : numbers)
  {
    elems.push_back(kv.second);
  }
  // ... and sort it by date, because it was not sorted in the map.
  std::sort(elems.begin(), elems.end(), [](const CovidNumbersElem& a, const CovidNumbersElem& b) {
        return a.date < b.date;
  });
  // Rebuild with daily values calculated by differences.
  CovidNumbers result;
  result.days.reserve(elems.size());
  for (std::size_t idx = 1; idx < elems.size(); ++idx)
  {
    CovidNumbersElem item;
    item.date = elems[idx].date;
    item.cases = elems[idx].cases - elems[idx-1].cases;
    item.deaths = elems[idx].deaths - elems[idx-1].deaths;
    item.incidence14 = std::numeric_limits<double>::quiet_NaN();
    result.days.push_back(item);
  }

  if (!elems.empty())
  {
    result.totalCases = elems.back().cases;
    result.totalDeaths = elems.back().deaths;
  }
  return result;
}

CovidNumbers requestHistoricalApi(const std::string& geoId, const bool all)
{
  const auto url = constructHistoricalApiUrl(geoId, "", all);
  const auto response = performApiRequest(url);
  if (!response.has_value())
    return CovidNumbers();

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response.value());
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from disease.sh API!" << std::endl
              << "Response is: " << response.value() << std::endl;
    return CovidNumbers();
  }

  return parseJsonTimeline(doc);
}

CovidNumbers requestHistoricalApiProvince(const std::string& geoId, const std::string& province, const bool all)
{
  const auto url = constructHistoricalApiUrl(geoId, province, all);
  const auto response = performApiRequest(url);
  if (!response.has_value())
    return CovidNumbers();

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response.value());
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from disease.sh API!" << std::endl
              << "Response is: " << response.value() << std::endl;
    return CovidNumbers();
  }

  return parseJsonTimeline(doc);
}

CovidNumbers requestHistoricalApiUsaCounties(const std::string& county, const bool all)
{
  const auto url = constructHistoricalApiUrlUsaCounties(county, all);
  const auto response = performApiRequest(url);
  if (!response.has_value())
    return CovidNumbers();

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response.value());
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from disease.sh API!" << std::endl
              << "Response is: " << response.value() << std::endl;
    return CovidNumbers();
  }

  // The API for US counties returns an array, where each element within is the
  // data for a province within the area. To get the total numbers, those have
  // to be added up.
  const auto [vec, error_array] = doc.get_array();
  if (error_array)
  {
    std::cerr << "Error: Found invalid JSON format in request for USA counties." << std::endl;
    return CovidNumbers();
  }


  bool mayNeedSorting = false;
  CovidNumbers numbers;
  for (auto elem : vec)
  {
    const auto partial_numbers = parseJsonTimeline(elem);
    if (partial_numbers.days.empty())
    {
      std::cerr << "Error: Got no data for one of the USA counties!" << std::endl;
      return CovidNumbers();
    }
    if (numbers.days.empty())
    {
      numbers = partial_numbers;
    }
    else
    {
      // Add it up to existing numbers.
      for (auto num : partial_numbers.days)
      {
        const auto pos = std::find_if(numbers.days.begin(), numbers.days.end(),
                         [&num] (const CovidNumbersElem& x) { return x.date == num.date; } );
        if (pos != numbers.days.end())
        {
          pos->cases += num.cases;
          pos->deaths += num.deaths;
        }
        else
        {
          numbers.days.push_back(num);
          // Vector may need to be sorted, because we do not know whether it is
          // still sorted after the push().
          mayNeedSorting = true;
        }
      }
      if ((partial_numbers.totalCases > 0) && (numbers.totalCases >= 0))
      {
        numbers.totalCases += partial_numbers.totalCases;
      }
      if ((partial_numbers.totalDeaths > 0) && (numbers.totalDeaths >= 0))
      {
        numbers.totalDeaths += partial_numbers.totalDeaths;
      }
    }
  }

  if (mayNeedSorting)
  {
    std::sort(numbers.days.begin(), numbers.days.end(),
              [](const CovidNumbersElem& a, const CovidNumbersElem& b) {
                  return a.date < b.date;
             });
  }
  return numbers;
}

CovidNumbers requestHistoricalApiFirstOfMultipleProvinces(const std::string& geoId, const std::string& provinces, const bool all)
{
  const auto url = constructHistoricalApiUrl(geoId, provinces, all);
  const auto response = performApiRequest(url);
  if (!response.has_value())
    return CovidNumbers();

  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(response.value());
  if (error)
  {
    std::cerr << "Error while trying to parse JSON response from disease.sh API!" << std::endl
              << "Response is: " << response.value() << std::endl;
    return CovidNumbers();
  }
  const auto [vec, error_array] = doc.get_array();
  if (error_array)
  {
    std::cerr << "Error: Found invalid JSON format in request for multiple provinces." << std::endl;
    return CovidNumbers();
  }
  if (vec.size() == 0)
  {
    std::cerr << "Error: Found empty JSON array in request for multiple provinces." << std::endl;
    return CovidNumbers();
  }
  return parseJsonTimeline(vec.at(0).value());
}

} // namespace

} // namespace
