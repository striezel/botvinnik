/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2024  Dirk Stolle

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
#include <cmath>
#include "../../../src/botvinnik/plugins/weather/OpenMeteo.hpp"

TEST_CASE("plugin Weather: weather data from Open-Meteo")
{
  using namespace bvn;

  SECTION("get_weather")
  {
    SECTION("attempt to get weather for location without data")
    {
      const Location location;
      const auto data = OpenMeteo::get_weather(location);

      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Location has no latitude and longitude!" );
    }

    SECTION("attempt to get weather for a valid location")
    {
      Location location;
      location.name = "Berlin";
      location.display_name = "Berlin, Germany";
      location.latitude = 52.52437;
      location.longitude = 13.41053;

      const auto data = OpenMeteo::get_weather(location);

      REQUIRE( data.has_value() );
      const auto weather = data.value();
      REQUIRE_FALSE( std::isnan(weather.temperature_celsius) );
      REQUIRE_FALSE( std::isnan(weather.apparent_temperature) );
      REQUIRE( weather.relative_humidity >= 0 );
      REQUIRE( weather.weather_code >= 0 );
      REQUIRE( weather.wind_speed >= 0.0 );
      REQUIRE( weather.wind_direction >= 0.0 );
      REQUIRE( weather.wind_direction <= 360.0 );
      REQUIRE_FALSE( std::isnan(weather.pressure) );
      REQUIRE_FALSE( std::isnan(weather.precipitation) );
    }
  }

  SECTION("parse_response")
  {
    SECTION("parse response containing data")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE( data.has_value() );
      const auto weather = data.value();

      REQUIRE( weather.temperature_celsius == 3.7 );
      REQUIRE( weather.apparent_temperature == 0.9 );
      REQUIRE( weather.relative_humidity == 88 );
      REQUIRE( weather.weather_code == 0 );
      REQUIRE( weather.wind_speed == 6.8 );
      REQUIRE( weather.wind_direction == 18.0 );
      REQUIRE( weather.pressure == 1007.0 );
      REQUIRE( weather.precipitation == 0.0 );
    }

    SECTION("failure: response is not valid JSON")
    {
      const std::string json = "{\"generationtime_ms\":0.47898293";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "The weather data request returned invalid JSON." );
    }

    SECTION("failure: response is not a JSON object")
    {
      const std::string json = "[0.4, 78, 98, 293]";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: Root element is not an object!" );
    }

    SECTION("failure: current element is not an object")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": "fails here"
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'current' element is missing not an object!" );
    }

    SECTION("failure: current element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'current' element is missing not an object!" );
    }

    SECTION("failure: temperature_2m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: temperature_2m element is missing or not a floating-point number!" );
    }

    SECTION("failure: temperature_2m element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": "This is not a number!",
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: temperature_2m element is missing or not a floating-point number!" );
    }

    SECTION("failure: apparent_temperature element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: apparent_temperature element is missing or not a floating-point number!" );
    }

    SECTION("failure: apparent_temperature element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": true,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: apparent_temperature element is missing or not a floating-point number!" );
    }

    SECTION("failure: relative_humidity_2m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: relative_humidity_2m element is missing or not an integer number!" );
    }

    SECTION("failure: relative_humidity_2m element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": false,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: relative_humidity_2m element is missing or not an integer number!" );
    }

    SECTION("failure: weather_code element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: weather_code element is missing or not an integer number!" );
    }

    SECTION("failure: weather_code element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": "fail here",
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: weather_code element is missing or not an integer number!" );
    }

    SECTION("failure: wind_speed_10m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_speed_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: wind_speed_10m element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": [],
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_speed_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: wind_direction_10m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_direction_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: wind_direction_10m element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": {}
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_direction_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: surface_pressure element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: surface_pressure element is missing or not a floating-point number!" );
    }

    SECTION("failure: surface_pressure element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": 0,
          "weather_code": 0,
          "surface_pressure": [ 1, 0, 0, 7 ],
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: surface_pressure element is missing or not a floating-point number!" );
    }

    SECTION("failure: precipitation element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: precipitation element is missing or not a floating-point number!" );
    }

    SECTION("failure: precipitation element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52, "longitude": 13.419998,
        "generationtime_ms": 0.12004375457763672,
        "utc_offset_seconds": 7200, "timezone": "Europe/Berlin",
        "timezone_abbreviation": "CEST", "elevation": 38,
        "current_units": {
          "time": "iso8601", "interval": "seconds",
          "temperature_2m": "°C", "relative_humidity_2m": "%",
          "apparent_temperature": "°C", "precipitation": "mm",
          "weather_code": "wmo code", "surface_pressure": "hPa",
          "wind_speed_10m": "km/h", "wind_direction_10m": "°"
        },
        "current": {
          "time": "2024-04-18T01:00",
          "interval": 900,
          "temperature_2m": 3.7,
          "relative_humidity_2m": 88,
          "apparent_temperature": 0.9,
          "precipitation": false,
          "weather_code": 0,
          "surface_pressure": 1007,
          "wind_speed_10m": 6.8,
          "wind_direction_10m": 18
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: precipitation element is missing or not a floating-point number!" );
    }
  }
}
