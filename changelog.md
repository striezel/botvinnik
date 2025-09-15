# Version history of botvinnik Matrix bot

_(Note: This changelog focuses on the major changes between the different
versions. Therefore, it may not contain all changes. Especially smaller fixes or
improvements may be omitted.)_

## Version 0.?.? (2025-09-??)

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 4.0.1 to version 4.0.2.

## Version 0.9.6 (2025-09-13)

* __[new feature]__
  The Debian package search is extended by adding the `!deb14` command to search
  for packages in Debian 14 (a.k.a. "forky").

* __[change]__
  Furthermore, the `!deb` command is now an alias for `!deb13` instead for
  `!deb12`. In other words: `!deb` does now search packages of Debian 13 instead
  of Debian 12 as before.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.12.2 to version 4.0.1.

* __[maintenance]__
  The library that does the JSON serialization (nlohmann/json) has been updated
  from version 3.11.3 to version 3.12.0.

* __[maintenance]__
  The base image for the Docker build is updated from `debian:12-slim` to
  `debian:13-slim`, i. e. from Debian 12 (codename "bookworm") to Debian 13
  (codename "trixie").

## Version 0.9.5 (2025-03-17)

* __[change]__
  The data source for the `!corona` command has been switched to the compact
  COVID-19 dataset of Our World in Data. Reason for that switch is that the
  previously used other dataset from Our World in Data no longer is updated.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.9.1 to version 3.12.2.

## Version 0.9.4 (2024-04-25)

* __[fix]__
  Fix a bug where the `!weather` command would show the wrong weather condition
  (icon + description) in the forecast part.

## Version 0.9.3 (2024-04-24)

* __[new feature]__
  The new `!weather` command is introduced. It can show the current weather and
  a weather forecast for the next few days for given locations. For example,
  `!weather Berlin` will show the current weather in Berlin, Germany, as well as
  a forecast for the next few days.

  The weather data for that command is provided by
  [Open-Meteo.com](https://open-meteo.com/).

* __[fix]__
  The bot will now handle configuration files with Windows-style line endings
  (carriage return and line feed) containing empty lines properly.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.8.0 to version 3.9.1.

## Version 0.9.2 (2024-03-12)

* __[feature]__
  Two more languages are now supported by the Wikipedia command: Japanese and
  Chinese. For those the new commands `!wikija` (for the Japanese Wikipedia)
  and `!wikizh` (for the Chinese Wikipedia) are added.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.7.0 to version 3.8.0.

## Version 0.9.1 (2024-03-09)

* __[fix]__
  Fix a bug where some Linux distributions (namely Alpine) could not properly
  detect the user's home directory and thus would not search for an existing
  configuration file in the user's home directory.

## Version 0.9.0 (2024-03-06)

* __[fix]__
  The `!fortune` command will now work on Linux distributions that are not based
  on Debian, too.

* __[change]__
  The `!fortune` command is now only available, if the bot runs on a Linux
  system. Technically, that is not a limitation, because the fortune binary is
  usually not available on Windows or macOS systems.

## Version 0.8.0 (2024-03-01)

* __[change]__
  The data source for the `!corona` command has been switched from the World
  Health Organization to Our World in Data. Reason for that switch is that the
  World Health Organization stopped providing daily data and only provides
  weekly data now.

  As part of that change, the option to specify a country by its two-letter
  country code according to ISO-3166 is changed to three-letter code instead.
  That's used in the data which is provided by Our World in Data, so the bot
  will use that from now on. The change means that e. g. when users previously
  used the command `!corona GR` to get the numbers for Greece, they will now
  have to use `!corona GRC` instead. Of course, full country names are still
  possible, too, so `!corona Greece` will also work in that case.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.6.3 to version 3.7.0.

## Version 0.7.0 (2023-12-10)

* __[new feature]__
  The `!help` command can now also show a detailed help for a specific command,
  if a command is named. For example, the command `!help wiki` or `!help !wiki`
  will show detailed help for the `!wiki` command.

  If the the `!help` command is sent without a command name it will list all
  available commands and a short(er) explanation for each command, just as in
  previous versions of the bot.

* __[improvement]__
  The images that the bot sends as response to the `!giphy` command will now
  have some metadata about the image size in bytes, the width and the height.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.2.3 to version 3.6.3.

* __[maintenance]__
  The library that does the JSON serialization (nlohmann/json) has been updated
  from version 3.11.2 to version 3.11.3.

## Version 0.6.3 (2023-09-01)

* __[new feature]__
  A new command to display random GIFs - `!giphy` - is added. For example, the
  command `!giphy cat` will show a random GIF of a cat.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.2.0 to version 3.2.3.

## Version 0.6.2 (2023-06-25)

* __[new feature]__
The Debian package search is extended by adding the `!deb13` command to search
for packages in Debian 13 (a.k.a. "trixie").

* __[change]__
Furthermore, the `!deb` command is now an alias for `!deb12` instead for
`!deb11`. In other words: `!deb` does now search packages of Debian 12 instead
of Debian 11 as before.

* __[improvement]__
  When requesting COVID-19 data for Kosovo via the `!corona` command, the
  country will now be correctly named "Kosovo" instead of "Kosovo[1]". This is a
  quirk in the WHO data, and the program will now work around it.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.1.7 to version 3.2.0.

* __[maintenance]__
  The base image for the Docker build is updated from `debian:11-slim` to
  `debian:12-slim`, i. e. from Debian 11 (codename "bullseye") to Debian 12
  (codename "bookworm").

## Version 0.6.1 (2023-05-03)

__[improvement]__

Remove superfluous / redundant data from database of COVID-19 data created by
the `!corona` command to speed up database creation.

Furthermore, the search algorithm for finding countries by name has been
improved to allow for some variants of longer, complex names.

## Version 0.6.0 (2023-04-30)

* __[change]__
  The data source for the `!corona` command has been switched from the Center
  for Systems Science and Engineering (CSSE) at Johns Hopkins University to the
  World Health Organization COVID-19 Dashboard.
  Reason for that switch is that Johns Hopkins University stopped the collection
  of new data on 10th March 2023. The WHO also provides data beyond that point.

  The change to WHO as data source affects a few countries / territories
  (two-letter country codes according to ISO-3166 are given in parenthesis for
   easier search / identification):

  * Bonaire, Saint Eustatius and Saba (BQ) is split into three parts which now
    can be queried individually:
    * Bonaire (XA)
    * Sint Eustatius (XB)
    * Saba (XC)
  * Cape Verde (CV) is now called Cabo Verde.
  * Cote d'Ivoire (CI) is now called Côte d’Ivoire.
  * French Guiana (GF) is added as new country / territory.
  * Guadeloupe (GP) is added as new country / territory.
  * Laos (LA) is now called Lao People's Democratic Republic.
  * Martinique (MQ) is added as new country / territory.
  * Mayotte (YT) is added as new country / territory.
  * Micronesia (FM) is now called Micronesia (Federated States of).
  * Nauru (NR) is added as new country / territory.
  * Niue (NU) is added as new country / territory.
  * Northern Mariana Islands (MP) is now called Northern Mariana Islands
    (Commonwealth of the).
  * Palestine (PS) is now called occupied Palestinian territory, including east
    Jerusalem.
  * Pitcairn Islands (PN) are added as new country / territory.
  * South Korea (KR) is now called Republic of Korea.
  * Moldova (MD) is now called Republic of Moldova.
  * Réunion (RE) is added as new country / territory.
  * Russia (RU) is now called Russian Federation.
  * Saint Barthélemy (BL) is added as new country / territory.
  * Saint Helena, Ascension and Tristan da Cunha (SH) is added as new country /
    territory.
  * Saint Martin (MF) is added as new country / territory.
    (The Dutch part of the island is still available as Sint Maarten (SX).)
  * Saint Pierre and Miquelon (PM) is added as new country / territory.
  * Syria (SY) is now called Syrian Arab Republic.
  * Unfortunately, the WHO does not provide separate data for Taiwan (TW), so
    no separate data for Taiwan can be queried at the moment.
  * United Kingdom (UK) is now The United Kingdom, and its country code is
    changed from UK to GB.
  * Venezuela (VE) is now called Venezuela (Bolivarian Republic of).
  * Unfortunately, the WHO does not provide separate data for West Sahara (EH),
    so no separate data for West Sahara can be queried at the moment.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 3.0.1 to version 3.1.7.

## Version 0.5.2 (2022-12-22)

* __[change]__
  The bot will now log a warning to standard output, if Synapse 1.62.0 or later
  is used as Matrix homeserver. A similar warning will occur, when the `!ping`
  command detects an unusually high latency.

  Reason for that change is that Synapse 1.62.0 changed the default value for
  the Synapse setting `sync_response_cache_duration` from zero to two minutes,
  which means that all synchronisation requests will be cached for that time by
  default - unless the server setting has been changed manually. This means
  that with those versions the bot may take up to two minutes to become aware of
  any commands send to it, because the homeserver caches responses instead of
  returning up-to-date messages. Server administrators are advised to set the
  `sync_response_cache_duration` value to zero or - if that is not possible - to
  a value not higher than a few seconds.

* __[maintenance]__
  The library that does the JSON parsing (simdjson) has been updated from
  version 2.2.2 to version 3.0.1.

## Version 0.5.1 (2022-10-03)

* __[change]__
  The default value for the configuration setting `bot.sync.delay_milliseconds`
  is changed from 5000 to 2500, i. e. the bot will now try to perform a
  synchronization request every 2.5 seconds instead of every five seconds, if
  that value is not set explicitly in the configuration file.

* __[change]__
  The upper limit for the `bot.sync.allowed_failures` configuration setting is
  is increased from 31 to 63. Its default value is also changed from twelve to
  24. That way a bot instance that uses default values for both the
  `bot.sync.allowed_failures` and the `bot.sync.delay_milliseconds` settings
  will still be able to endure a network downtime of approx. one minute before
  it fails, as in previous versions.

## Version 0.5.0 (2022-09-26)

* __[new feature]__
  Bot commands can now be deactivated selectively via the configuration file.
  For that purpose the new configuration option `command.deactivate` is
  introduced.
  For example, the line `command.deactivate=ping` in the configuration file
  would deactivate the `!ping` command. This option can be given multiple times
  to deactivate more than one command.

  _Note:_ Some commands like `!help`, `!stop` and others that are essential to
  operate the bot cannot be deactivated. The bot will not start if you attempt
  to deactivate such a command via the configuration file.

* __[new feature]__
  A new configuration option, `bot.sync.delay_milliseconds`, can now be used to
  specify the delay between two consecutive Matrix synchronization requests. In
  previous versions that value was hard-coded to 5000 ms (i. e. 5 seconds), and
  that is its default value, if it is not specified in the configuration file.

  Primary purpose of the new setting is to allow a balance between too high load
  on the Matrix server due to too many synchronization requests from the bot on
  the one side and a bot that is responding to commands too slowly on the other
  side.

* __[maintenance]__
  The library that does the JSON serialization (nlohmann/json) has been updated
  from version 3.11.1 to version 3.11.2.

## Version 0.4.1 (2022-08-06)

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
1.0.2 to version 2.2.2.

* __[maintenance]__
The library that does the JSON serialization (nlohmann/json) has been updated
from version 3.10.5 to version 3.11.1.

* __[maintenance]__
The base image for the Docker build is updated from `debian:10-slim` to
`debian:11-slim`, i. e. from Debian 10 (codename "buster") to Debian 11
(codename "bullseye").

## Version 0.4.0 (2022-05-03)

__[new feature]__
A new feature for text translation and two corresponding commands are added.
The command `!tr` does the translation, e. g. typing `!tr en fr Hello world!`
will translate the text "Hello world!" from English ("en") to French ("fr").
The command `!tr-lang` will show a list of available languages and their
corresponding language codes.

To enable the translation one has to add the URL to a LibreTranslate server as
well as an API key (only if the server requires a key) to the bot's
configuration file, e. g.

    libretranslate.server=https://libretranslate.example.com
    libretranslate.apikey=your-key-here

A list of some possible servers is available at
<https://github.com/LibreTranslate/LibreTranslate#mirrors>.

## Version 0.3.16 (2022-05-03)

* __[change]__
The `!version` command does now contain information about the version of the
SQLite 3 library, too. The same information is included when invoking the
executable with the `--version` parameter on the command line.

* __[new feature]__
The Debian package search is extended by adding the `!deb12` command to search
for packages in Debian 12 (a.k.a. "bookworm").

* __[change]__
Furthermore, the `!deb` command is now an alias for `!deb11` instead for
`!deb10`. In other words: `!deb` does now search packages of Debian 11 instead
of Debian 10 as before.

## Version 0.3.15 (2022-01-26)

__[improvement]__
Case numbers for American Samoa, Federated States of Micronesia, Kiribati,
Palau, Samoa and Tonga can now be queried via the `!corona` command, too.

## Version 0.3.14 (2022-01-13)

* __[maintenance]__
The library that does the JSON serialization (nlohmann/json) has been updated
from version 3.10.4 to version 3.10.5.

* __[fix]__
The `!xkcd` command will now automatically check for newer available comics, if
the last check was more than 12 hours ago. This fixes a long-standing bug where
the bot would not recognise newer xkcd comics, if it was running for an extended
period of time.

## Version 0.3.13 (2021-11-06)

* __[change]__
The `!corona` command switches to canonical URL of disease.sh API for its
requests. The plugin does now use `https://disease.sh/` as base URL for API
requests. This seems to be more reliable than the previously used alternative
base URL `https://corona.lmao.ninja/` which had problems with Cloudflare (like
502 Bad Gateway, etc.) rather often, so that requests sometimes failed due to
outages of it or problems with Cloudflare.

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
1.0.0 to version 1.0.2. This fixes an error with parsing large 64-bit integers.
Usually, such big numbers should not occur inside the Matrix protocol's JSON,
but let's better be on the safe side here. It also patches a bug when iterating
over JSON arrays containing objects.

* __[maintenance]__
The library that does the JSON serialization (nlohmann/json) has been updated
from version 3.10.2 to version 3.10.4.

## Version 0.3.12 (2021-10-01)

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
0.9.1 to version 1.0.0.

* __[maintenance]__
The library that does the JSON serialization (nlohmann/json) has been updated
from version 3.9.1 to version 3.10.2.

## Version 0.3.11 (2021-09-09)

__[fix]__
This release fixes a memory leak in the URL-encoding algorithm.

## Version 0.3.10 (2021-07-28)

* __[fix]__
An HTML injection vulnerability in the implementation of the `!cheats` command
is fixed.

Other commands where similar code injections would be possible at least in
theory have been secured, too. Those are the commands `!deb`, `!help`,
`!fortune`, and `!xkcd`. While I believe that most of these other commands are
not vulnerable in practice, because the input is under the program's control
(command `!help`) or is known to not contain HTML code (e. g. `!deb`) it is
better to take precautions.

## Version 0.3.9 (2021-07-15)

__[new feature]__
A new command to show cheat sheets on various topics - `!cheat` - is added. The
cheat sheets are mainly about Unix programs and programming languages.

## Version 0.3.8 (2021-07-13)

* __[change]__
The `!version` does now contains information about the used libraries, too. The
same information is included when invoking the executable with the `--version`
parameter on the command line.

* __[improvement]__
HTTPS connections will now try to use TLS v1.2 or newer by default, if your
system uses cURL 7.54.0 or newer. For older cURL versions, the minimum TLS
version is 1.0 as it was before. However, even on older cURL versions that does
not prevent botvinnik from using newer TLS versions (e. g. v1.2 or v1.3) as
long as the SSL library supports newer TLS versions.

* __[improvement]__
Furthermore, the networking code has been stripped of unused parts, thus
simplifying the code that interacts with the cURL library.

## Version 0.3.7 (2021-07-11)

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
0.7.1 to version 0.9.1.

* __[fix]__
The `!rooms` command will now display room names even in cases where one of the
rooms has no name.

## Version 0.3.6 (2021-07-11)

* __[improvement]__
The parsing of Matrix power levels is now stricter, allowing less deviation from
the Matrix specification.

* __[improvement]__
The parsing of room events is now more tolerant to missing optional JSON
members that can be left out, according to the Matrix specification.

* __[fix]__
Due to a typo in previous versions, the `!corona` command did not properly
recognize the country Ivory Coast. One had to type `!corona Cote dIvoire` to get
the numbers. This is now changed, `!corona Cote d'Ivoire` will now show the
numbers for that country, because that is the correct spelling. As before, you
can also use the ISO 3166 country code, i. e. `!corona CI` will also show the
data for Cote d'Ivore.

## Version 0.3.5 (2021-06-18)

* __[change]__
The `!corona` command will now recognize the Cook Islands as a separate country.
Case numbers have previously been part of New Zealand's numbers, but they are
not anymore.

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
0.7.0 to version 0.7.1.

## Version 0.3.4 (2021-05-30)

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
0.6.0 to version 0.7.0.

* __[maintenance]__
The library that does the JSON serialization (nlohmann/json) has been updated
from version 3.5.0 to version 3.9.1.

* __[fix]__
Add missing `#include` statement that could lead to build error on some
compilers.

* __[improvement]__
Add workaround for gateway timeout of test server.

## Version 0.3.3 (2021-04-27)

__[change]__
The minimum required CMake version for compiling the project is raised from 2.8
to 3.8.

## Version 0.3.2 (2021-04-15)

__[fix]__
A rare case where determining the user's home directory could lead to reading
uninitialized data is fixed. While it is unlikely that a normal user has no
entry in the user database, it can happen. That case was not handled properly,
and therefore could lead to reading uninitialized data.

## Version 0.3.1 (2021-03-25)

__[change]__
The incidence values of the `!corona` command are changed from the previous
14-day incidence to 7-day incidence values.

## Version 0.3.0 (2021-01-25)

__[fix]__
The command `!corona` does now work again. Its data source has been switched
from the European Centre for Disease Prevention and Control (ECDC) to the
Center for Systems Science and Engineering (CSSE) at Johns Hopkins University.
Reason for that switch is the ECDC's change in the published data: Instead of
daily data it started publishing bi-weekly data after 14th December 2020.
However, the bot expects daily data to show detailed information.

## Version 0.2.6 (2020-12-10)

__[improvement]__
The command `!corona` will now show the 14-day incidence values, too, if they
are available. These values are better suited than absolute case numbers to
compare the severity of the pandemic between countries.

## Version 0.2.5 (2020-11-25)

* __[new feature]__
  The bot can now send a User-Agent header to be easily identified as bot by
  the Matrix server. This behaviour is disabled by default. However, it can be
  enabled during the build by using the flag `USER_AGENT` for CMake, e. g.

      cmake -DUSER_AGENT=ON

  instead of just

      cmake

* __[new feature]__
  When the bot is invited into an encrypted room, it will now post a message
  explaining it cannot handle encrypted rooms (yet) and will then leave the
  room. Being in a room that uses encryption is no use to the bot anyway,
  because it cannot read those messages (yet). That may or may not change in
  the future.
* __[improvement]__
  The Matrix client has been hardened against unexpected values by using more
  URL-encoding on parameters that are supplied by the server. This may not be
  necessary for the default Matrix server implementation (Synapse), but it might
  help when someone uses another Matrix server implementation.

## Version 0.2.4 (2020-11-10)

* __[maintenance]__
The library that does the JSON parsing (simdjson) has been updated from version
0.4.6 to version 0.6.0.

* __[improvement]__
Furthermore, a note has been added to clarify how the `!ping` command works and
why it can take up to five seconds to answer.

## Version 0.2.3 (2020-11-04)

* __[fix]__
A build error with GNU g++ 10 is fixed, so the code will now build with g++ 10,
too.

* __[maintenance]__
Furthermore, the library that does the JSON parsing (simdjson) has been updated
from version 0.3.1 to version 0.4.6.

## Version 0.2.2 (2020-10-28)

__[improvement]__
The command `!xkcd` does now try to reuse existing uploaded media when the same
comic has already been uploaded before, thus reducing the amount of media data
on the side of the Matrix server. As a side effect, the bot is also faster to
respond when using existing media content, because it does not need to be
uploaded again.

## Version 0.2.1 (2020-09-27)

__[improvement]__
The command for showing the current COVID-19 data will now send an informational
message when the data is updated, because that can take a moment and the user
may otherwise think that the bot has crashed, because it is not responding
within the usual timeframe.

## Version 0.2.0 (2020-09-27)

__[new feature]__
An additional configuration option, `bot.sync.allowed_failures`, can now be used
to specify a tolerance to short-time Matrix synchronization failures. This is
useful when the network connection may occasionally be down for a few seconds.
Before that, the bot would stop instantly, if the network connection failed,
resulting in the need to do a manual restart for every short network outage.

## Version 0.1.11 (2020-09-02)

__[improvement]__
Debian package search is now case-insensitive. That is, even if someone enters
upper case letters, the proper lower case will be used for the package search.

## Version 0.1.10 (2020-08-30)

* __[improvement]__
  COVID-19 data will now show the numbers of the last ten days instead of just
  the last seven days.
* __[new feature]__
  Additional to just showing the data for a single country, the COVID-19 data
  for the whole world can now be shown by typing `!corona world` or
  `!corona all`. Country-specific numbers can still be shown as before by giving
  the two-letter country code or the English name of the country, e. g. by
  typing `!corona Germany` or `!corona DE` for the numbers of Germany.

## Version 0.1.9 (2020-08-23)

* __[new feature]__
The `!leave` command now has a second form that does not require a room id.
When invoked without a Matrix room id, the bot is told to leave the Matrix room
where the command was sent.

* __[improvement]__
Furthermore, not only the users who are allowed to stop the bot can make the bot
leave a room. The bot will also obey the request, if the requesting user has
the power level to ban or kick users out of the corresponding Matrix room.

## Version 0.1.8 (2020-07-25)

* __[improvement]__
  CSV parsing for COVID-19 data is improved:
    * The format of the CSV data has been extended with a new column, so the
      parsing needs to be adjusted, too. However, the new column for the number
      of COVID-19 cases per 100000 inhabitants for the last 14 days is currently
      ignored and not evaluated in any way.
    * Data for 'Bonaire, Saint Eustatius and Saba' (BQ) is now parsed correctly.
* __[improvement]__
  The bot will now also forget a room's history after it left the room.
* __[new feature]__
  The xkcd command does now accept a number indicating a specific comic. For
  example `!xkcd 1` will show the very first xkcd comic. Omitting the number or
  specifying something that is not a number will just display a random comic,
  i. e. shows the same behaviour as before.

## Version 0.1.7 (2020-06-24)

__[new feature]__
The bot can now automatically join Matrix rooms when invited. New room-related
commands (`!rooms` and `!leave`) are added.

* Any user can now invite the bot to a room of their choice. The bot will then
  attempt to join the room automatically. Note that encrypted rooms have not
  been tested yet and probably will not work with those invites.
* Furthermore, two room-related commands have been added: one to show the rooms
  that the bot has joined, and one to force the bot to leave a specified room.
  Both commands are only available to those users that are allowed to stop the
  bot. That prevents "random" room leaves and avoids to disclose room ids to
  any user.

## Version 0.1.6 (2020-06-20)

__[improvement]__
Improve CSV parsing for COVID-19 data in two aspects:

* Year of population data can now be anywhere from 2000 to 2999. (CSV header
  has changed from 2018 to 2019 for population data, but let's be safe for a bit
  longer here.)
* Empty case numbers will be interpreted as zero instead of aborting the parsing
  process.

## Version 0.1.5 (2020-06-17)

* __[new feature]__ Add numeric conversion commands to convert between
  hexadecimal, decimal and binary numbers.
* __[improvement]__ Output formatting for coronavirus information is improved.

## Version 0.1.4 (2020-06-08)

__[new feature]__
Add a new command to retrieve case numbers by country for COVID-19 (SARS-CoV-2,
or just "novel coronavirus").

## Version 0.1.3 (2020-06-01)

__[new feature]__
A new command to search for Debian packages is added.

## Version 0.1.2 (2020-05-29)

__[improvement]__
Improvements for the command to retrieve comics from xkcd.com are made.

## Version 0.1.1 (2020-05-29)

__[new feature]__
A first (experimental) version of the `!xkcd` command is added.

## Version 0.1.0 (2020-05-28)

__[new feature]__
More languages are now supported by the Wikipedia command.

The following languages are added: Czech, Dutch, Greek, Polish, Portuguese,
Turkish, Ukrainian, and Welsh.

## Version 0.0.11 (2020-05-28)

__[fix]__
Only allow users listed in the configuration file to stop the bot.
Otherwise just any user could type the stop command to disrupt the normal
operation of the bot.

## Version 0.0.10 (2020-05-27)

__[new feature]__
The `!fortune` command to show a random fortune is added. It requires the
fortune binary to be available at `/usr/games/fortune`. It is usually enough to
do a `apt-get install fortune-mod fortunes-min` on Debian-based systems to make
the binary and some basic data files available.

## Version 0.0.9 (2020-05-27)

__[new feature]__
The `!ping` command to show the time it took the message to reach the bot is
added.

## Version 0.0.8 (2020-05-27)

* __[new feature]__
  The `!whoami` command to show the current user is added. (This is more of a
  test command than an actual useful command.)
* __[new feature]__ A Dockerfile for a multi-stage Docker build is added.

## Version 0.0.7 (2020-05-25)

__[new feature]__
The first version of the Wikipedia bot command is added. It can be used to query
article information from the English, French, German, Italian, Spanish or
Russian Wikipedia.

## Version 0.0.6 and earlier

You do not want to use these old versions. They provide too little stuff that
can be used.
