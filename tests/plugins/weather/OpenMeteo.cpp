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
      REQUIRE_FALSE( std::isnan(weather.current.temperature_celsius) );
      REQUIRE_FALSE( std::isnan(weather.current.apparent_temperature) );
      REQUIRE( weather.current.relative_humidity >= 0 );
      REQUIRE( weather.current.weather_code >= 0 );
      REQUIRE( weather.current.wind_speed >= 0.0 );
      REQUIRE( weather.current.wind_direction >= 0.0 );
      REQUIRE( weather.current.wind_direction <= 360.0 );
      REQUIRE_FALSE( std::isnan(weather.current.pressure) );
      REQUIRE_FALSE( std::isnan(weather.current.precipitation) );
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE( data.has_value() );
      const auto weather = data.value();

      REQUIRE( weather.current.temperature_celsius == 3.7 );
      REQUIRE( weather.current.apparent_temperature == 0.9 );
      REQUIRE( weather.current.relative_humidity == 88 );
      REQUIRE( weather.current.weather_code == 0 );
      REQUIRE( weather.current.wind_speed == 6.8 );
      REQUIRE( weather.current.wind_direction == 18.0 );
      REQUIRE( weather.current.pressure == 1007.0 );
      REQUIRE( weather.current.precipitation == 0.0 );

      REQUIRE( weather.forecast.size() == 7 );
      REQUIRE( weather.forecast[0].date == "2024-04-24" );
      REQUIRE( weather.forecast[1].date == "2024-04-25" );
      REQUIRE( weather.forecast[2].date == "2024-04-26" );
      REQUIRE( weather.forecast[3].date == "2024-04-27" );
      REQUIRE( weather.forecast[4].date == "2024-04-28" );
      REQUIRE( weather.forecast[5].date == "2024-04-29" );
      REQUIRE( weather.forecast[6].date == "2024-04-30" );
      REQUIRE( weather.forecast[0].weather_code == 80 );
      REQUIRE( weather.forecast[1].weather_code == 80 );
      REQUIRE( weather.forecast[2].weather_code == 3 );
      REQUIRE( weather.forecast[3].weather_code == 2 );
      REQUIRE( weather.forecast[4].weather_code == 3 );
      REQUIRE( weather.forecast[5].weather_code == 1 );
      REQUIRE( weather.forecast[6].weather_code == 3 );
      REQUIRE( weather.forecast[0].temperature_max == 9.5 );
      REQUIRE( weather.forecast[1].temperature_max == 9.9 );
      REQUIRE( weather.forecast[2].temperature_max == 14.6 );
      REQUIRE( weather.forecast[3].temperature_max == 19.5 );
      REQUIRE( weather.forecast[4].temperature_max == 23.1 );
      REQUIRE( weather.forecast[5].temperature_max == 25.2 );
      REQUIRE( weather.forecast[6].temperature_max == 25.2 );
      REQUIRE( weather.forecast[0].temperature_min == 3.3 );
      REQUIRE( weather.forecast[1].temperature_min == 1.5 );
      REQUIRE( weather.forecast[2].temperature_min == 2.8 );
      REQUIRE( weather.forecast[3].temperature_min == 7.0 );
      REQUIRE( weather.forecast[4].temperature_min == 9.8 );
      REQUIRE( weather.forecast[5].temperature_min == 13.1 );
      REQUIRE( weather.forecast[6].temperature_min == 14.4 );
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
        "current": "fails here",
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'current' element is missing or not an object!" );
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'current' element is missing or not an object!" );
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: relative_humidity_2m element is missing or not an integer number!" );
    }

    SECTION("failure: current.weather_code element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: weather_code element is missing or not an integer number!" );
    }

    SECTION("failure: current.weather_code element is not a number")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: weather_code element is missing or not an integer number!" );
    }

    SECTION("failure: current.wind_speed_10m element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_speed_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.wind_speed_10m element is not a number")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_speed_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.wind_direction_10m element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_direction_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.wind_direction_10m element is not a number")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: wind_direction_10m element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.surface_pressure element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: surface_pressure element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.surface_pressure element is not a number")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: surface_pressure element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.precipitation element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: precipitation element is missing or not a floating-point number!" );
    }

    SECTION("failure: current.precipitation element is not a number")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: precipitation element is missing or not a floating-point number!" );
    }

    SECTION("failure: daily element is not an object")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": "fails here, not an object"
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'daily' element is missing or not an object!" );
    }

    SECTION("failure: daily element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: 'daily' element is missing or not an object!" );
    }

    SECTION("failure: daily.time element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.time element is missing or not an array!" );
    }

    SECTION("failure: daily.time element is not an array")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": true,
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.time element is missing or not an array!" );
    }

    SECTION("failure: daily.time element is an empty array")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.time element is an empty array!" );
    }

    SECTION("failure: daily.time contains an element which is not a string")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", false, "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: An element of daily.time is not a string!" );
    }

    SECTION("failure: daily.weather_code element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.weather_code element is missing or not an array!" );
    }

    SECTION("failure: daily.weather_code element is not an array")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": 400,
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.weather_code element is missing or not an array!" );
    }

    SECTION("failure: daily.weather_code array size does not match daily.time's size")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.weather_code array has an unexpected number of elements!" );
    }

    SECTION("failure: daily.weather_code contains an element with wrong type")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, "3", 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: An element of daily.weather_code is not an integer!" );
    }

    SECTION("failure: daily.temperature_2m_max element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_max element is missing or not an array!" );
    }

    SECTION("failure: daily.temperature_2m_max element is not an array")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": 9.5,
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_max element is missing or not an array!" );
    }

    SECTION("failure: daily.temperature_2m_max array size does not match daily.time's size")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2, 27.3 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_max array has an unexpected number of elements!" );
    }

    SECTION("failure: daily.temperature_2m_max contains an element with wrong type")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, "25.2", 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: An element of daily.temperature_2m_max is not a floating-point number!" );
    }

    SECTION("failure: daily.temperature_2m_min element is missing")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_min element is missing or not an array!" );
    }

    SECTION("failure: daily.temperature_2m_min element is not an array")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": 3.3
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_min element is missing or not an array!" );
    }

    SECTION("failure: daily.temperature_2m_min array size does not match daily.time's size")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ 3.3, 1.5, 2.8 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: daily.temperature_2m_min array has an unexpected number of elements!" );
    }

    SECTION("failure: daily.temperature_2m_min contains an element with wrong type")
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
        },
        "daily_units": {
          "time": "iso8601", "weather_code": "wmo code",
          "temperature_2m_max": "°C", "temperature_2m_min": "°C"
        },
        "daily": {
          "time": [ "2024-04-24", "2024-04-25", "2024-04-26", "2024-04-27", "2024-04-28", "2024-04-29", "2024-04-30" ],
          "weather_code": [ 80, 80, 3, 2, 3, 1, 3 ],
          "temperature_2m_max": [ 9.5, 9.9, 14.6, 19.5, 23.1, 25.2, 25.2 ],
          "temperature_2m_min": [ false, 1.5, 2.8, 7, 9.8, 13.1, 14.4 ]
        }
      }
      )json";

      const auto data = OpenMeteo::parse_response(json);
      REQUIRE_FALSE( data.has_value() );
      REQUIRE( data.error() == "Open-Meteo request returned invalid JSON: An element of daily.temperature_2m_min is not a floating-point number!" );
    }
  }
}
