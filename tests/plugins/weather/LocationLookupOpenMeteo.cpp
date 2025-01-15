/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2024, 2025  Dirk Stolle

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

#include "../../locate_catch.hpp"
#include "../../../src/botvinnik/plugins/weather/LocationLookupOpenMeteo.hpp"

TEST_CASE("plugin Weather: location lookup via Open-Meteo")
{
  using namespace bvn;

  SECTION("find_location")
  {
    SECTION("attempt to find location that does not exist")
    {
      const auto location = LocationLookupOpenMeteo::find_location("Waaaaaaargablah");

      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "No matching location was found." );
    }

    SECTION("find existing location")
    {
      const auto location = LocationLookupOpenMeteo::find_location("Leipzig");

      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 50.9 );
      REQUIRE( data.latitude <= 51.7 );

      REQUIRE( data.longitude >= 12.0 );
      REQUIRE( data.longitude <= 12.6 );

      REQUIRE( data.name == "Leipzig" );
      REQUIRE( data.display_name == "Leipzig, Saxony, Germany" );
    }

    SECTION("find existing location, part 2")
    {
      const auto location = LocationLookupOpenMeteo::find_location("Strasbourg");

      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 48.3 );
      REQUIRE( data.latitude <= 48.9 );

      REQUIRE( data.longitude >= 7.5 );
      REQUIRE( data.longitude <= 7.9 );

      REQUIRE( data.name == "Strasbourg" );
      REQUIRE( data.display_name == "Strasbourg, Grand Est, France" );
    }
  }

  SECTION("parse_response")
  {
    SECTION("parse response containing data")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": "Pisa",
            "latitude": 43.70853,
            "longitude": 10.4036,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 43.7 );
      REQUIRE( data.latitude <= 43.8 );

      REQUIRE( data.longitude >= 10.4 );
      REQUIRE( data.longitude <= 10.5 );

      REQUIRE( data.name == "Pisa" );
      REQUIRE( data.display_name == "Pisa, Tuscany, Italy" );
    }

    SECTION("parse response without matching data")
    {
      const std::string json = R"json(
        {"generationtime_ms":0.47898293}
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "No matching location was found." );
    }

    SECTION("failure: response is not valid JSON")
    {
      const std::string json = "{\"generationtime_ms\":0.47898293";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "The location lookup returned invalid JSON." );
    }

    SECTION("failure: response is not a JSON object")
    {
      const std::string json = "[0.4, 78, 98, 293]";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: Root element is not an object!" );
    }

    SECTION("failure: results element is not an array")
    {
      const std::string json = R"json(
      {
        "results": "not-an-object",
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: results element is not an array!" );
    }

    SECTION("failure: name element is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "latitude": 43.70853,
            "longitude": 10.4036,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: name element is missing or not a string!" );
    }

    SECTION("failure: name element is not a string")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": [ "Pisa", "but as array" ],
            "latitude": 43.70853,
            "longitude": 10.4036,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: name element is missing or not a string!" );
    }

    SECTION("failure: latitude element is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": "Pisa",
            "longitude": 10.4036,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: latitude is missing or not a floating-point number!" );
    }

    SECTION("failure: latitude element is not a floating-point number")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": "Pisa",
            "latitude": "43.70853 is a string",
            "longitude": 10.4036,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: latitude is missing or not a floating-point number!" );
    }

    SECTION("failure: longitude element is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": "Pisa",
            "latitude": 43.70853,
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: longitude is missing or not a floating-point number!" );
    }

    SECTION("failure: longitude element is not a floating-point number")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 3170647,
            "name": "Pisa",
            "latitude": 43.70853,
            "longitude": "I am a string - 10.4036",
            "elevation": 4,
            "feature_code": "PPLA2",
            "country_code": "IT",
            "admin1_id": 3165361,
            "admin2_id": 3170646,
            "admin3_id": 6542122,
            "timezone": "Europe/Rome",
            "population": 77007,
            "country_id": 3175395,
            "country": "Italy",
            "admin1": "Tuscany",
            "admin2": "Pisa",
            "admin3": "Pisa"
          }
        ],
        "generationtime_ms": 0.9469986
      }
      )json";

      const auto location = LocationLookupOpenMeteo::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "Open-Meteo lookup contained invalid JSON: longitude is missing or not a floating-point number!" );
    }
  }
}
