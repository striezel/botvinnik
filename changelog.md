# Version history of botvinnik Matrix bot

_(Note: This changelog focuses on the major changes between the different
versions. Therefore, it may not contain all changes. Especially smaller fixes or
improvements may be omitted.)_

## Next version (2020-09-??)

* COVID-19 data will now show the numbers of the last ten days instead of just
  the last seven days.
* Additional to just showing the data for a single country, the COVID-19 data
  for the whole world can now be shown by typing `!corona world` or
  `!corona all`. Country-specific numbers can still be shown as before by giving
  the two-letter country code or the English name of the country, e. g. by
  typing `!corona Germany` or `!corona DE` for the numbers of Germany.

## Version 0.1.9 (2020-08-23)

The `!leave` command now has a second form that does not require a room id.
When invoked without a Matrix room id, the bot is told to leave the Matrix room
where the command was sent.

Furthermore, not only the users who are allowed to stop the bot can make the bot
leave a room. The bot will also obey the request, if the requesting user has
the power level to ban or kick users out of the corresponding Matrix room.

## Version 0.1.8 (2020-07-25)

* CSV parsing for COVID-19 data is improved:
    * The format of the CSV data has been extended with a new column, so the
      parsing needs to be adjusted, too. However, the new column for the number
      of COVID-19 cases per 100000 inhabitants for the last 14 days is currently
      ignored and not evaluated in any way.
    * Data for 'Bonaire, Saint Eustatius and Saba' (BQ) is now parsed correctly.
* The bot will now also forget a room's history after it left the room.
* The xkcd command does now accept a number indicating a specific comic. For
  example `!xkcd 1` will show the very first xkcd comic. Omitting the number or
  specifying something that is not a number will just display a random comic,
  i. e. shows the same behaviour as before.

## Version 0.1.7 (2020-06-24)

The bot can now automatically joins Matrix rooms when invited. New room-related
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

Improve CSV parsing for COVID-19 data in two aspects:

* Year of population data can now be anywhere from 2000 to 2999. (CSV header
  has changed from 2018 to 2019 for population data, but let's be safe for a bit
  longer here.)
* Empty case numbers will be interpreted as zero instead of aborting the parsing
  process.

## Version 0.1.5 (2020-06-17)

* Add numeric conversion commands to convert between hexadecimal, decimal and
  binary numbers.
* Output formatting for coronavirus information is improved.

## Version 0.1.4 (2020-06-08)

Add a new command to retrieve case numbers by country for COVID-19 (SARS-CoV-2,
or just "novel coronavirus").

## Version 0.1.3 (2020-06-01)

A new command to search for Debian packages is added.

## Version 0.1.2 (2020-05-29)

Improvements for the command to retrieve comics from xkcd.com are made.

## Version 0.1.1 (2020-05-29)

A first (experimental) version of the `!xkcd` command is added.

## Version 0.1.0 (2020-05-28)

More languages are now supported by the Wikipedia command.

The following languages are added: Czech, Dutch, Greek, Polish, Portuguese,
Turkish, Ukrainian, and Welsh.

## Version 0.0.11 (2020-05-28)

Only allow users listed in the configuration file to stop the bot.
Otherwise just any user could type the stop command to disrupt the normal
operation of the bot.

## Version 0.0.10 (2020-05-27)

The `!fortune` command to show a random fortune is added. It requires the
fortune binary to be available at /usr/games/fortune. It is usually enough to
do a `apt-get install fortune-mod fortunes-min` on Debian-based systems to make
the binary and some basic data files available.

## Version 0.0.9 (2020-05-27)

The `!ping` command to show the time it took the message to reach the bot is
added.

## Version 0.0.8 (2020-05-27)

* The `!whoami` command to show the current user is added. (This is more of a
  test command than an actual useful command.)
* A Dockerfile for a multi-stage Docker build is added.

## Version 0.0.7 (2020-05-25)

The first version of the Wikipedia bot command is added. It can be used to query
article information from the English, French, German, Italian, Spanish or
Russian Wikipedia.

## Version 0.0.6 and earlier

You do not want to use these old versions.
