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
#include "../../../src/botvinnik/plugins/weather/LocationLookupOpenStreetMap.hpp"

TEST_CASE("plugin Weather: location lookup via OpenStreetMap")
{
  using namespace bvn;

  SECTION("find_location")
  {
    SECTION("attempt to find location that does not exist")
    {
      const auto location = LocationLookupOpenStreetMap::find_location("Waaaaaaargablah");

      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "No matching location was found." );
    }

    SECTION("find existing location")
    {
      const auto location = LocationLookupOpenStreetMap::find_location("Leipzig");

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
      const auto location = LocationLookupOpenStreetMap::find_location("Berlin, Camden");

      REQUIRE( location.has_value() );
      const auto data = location.value();

      REQUIRE( data.latitude >= 39.7 );
      REQUIRE( data.latitude <= 39.9 );

      REQUIRE( data.longitude >= -75.1 );
      REQUIRE( data.longitude <= -74.7 );

      REQUIRE( data.name == "Berlin" );
      REQUIRE( data.display_name == "Berlin, Camden County, New Jersey, United States" );
    }
  }

  SECTION("parse_response")
  {
    SECTION("parse response containing data")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
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
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": []
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "No matching location was found." );
    }

    SECTION("failure: response is not valid JSON")
    {
      const std::string json = "{\"type\": \"FeatureCo";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "The location lookup returned invalid JSON." );
    }

    SECTION("failure: response is not a JSON object")
    {
      const std::string json = "[0.4, 78, 98, 293]";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: Root element is not an object!" );
    }

    SECTION("failure: features element is missing")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "..."
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: features element is missing or not an array!" );
    }

    SECTION("failure: features element is not an array")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "some text",
        "features": "fails here"
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: features element is missing or not an array!" );
    }

    SECTION("failure: display_name element is missing")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: display_name element is missing or not a string!" );
    }

    SECTION("failure: display_name element is not a string")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": [ "Pisa", "Tuscany", "Italy" ]
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: display_name element is missing or not a string!" );
    }

    SECTION("failure: name element is missing")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: name element is missing or not a string!" );
    }

    SECTION("failure: name element is not a string")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": true,
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: name element is missing or not a string!" );
    }

    SECTION("failure: longitude is missing")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": []
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: longitude data is missing or not a floating-point number!" );
    }

    SECTION("failure: longitude is not a floating-point number")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                "this is a string and not a number",
                43.7159395
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: longitude data is missing or not a floating-point number!" );
    }

    SECTION("failure: latitude is missing")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: latitude data is missing or not a floating-point number!" );
    }

    SECTION("failure: latitude is not a floating-point number")
    {
      const std::string json = R"json(
      {
        "type": "FeatureCollection",
        "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http://osm.org/copyright",
        "features": [
          {
            "type": "Feature",
            "properties": {
              "place_id": 69313499,
              "osm_type": "relation",
              "osm_id": 42527,
              "place_rank": 16,
              "category": "boundary",
              "type": "administrative",
              "importance": 0.62612005641941,
              "addresstype": "city",
              "name": "Pisa",
              "display_name": "Pisa, Tuscany, Italy"
            },
            "bbox": [ 10.2684553, 43.5807231, 10.4562831, 43.7509347 ],
            "geometry": {
              "type": "Point",
              "coordinates": [
                10.4018624,
                "fails here"
              ]
            }
          }
        ]
      }
      )json";

      const auto location = LocationLookupOpenStreetMap::parse_response(json);
      REQUIRE_FALSE( location.has_value() );
      REQUIRE( location.error() == "OSM lookup contained invalid JSON: latitude data is missing or not a floating-point number!" );
    }
  }
}
