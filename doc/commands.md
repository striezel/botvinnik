# botvinnik: Available bot commands

botvinnik provides a (still rather small) number of commands for Matrix users
in the channels where the bot has joined. The bot automatically joins channels,
if the invitation takes place when the bot is running.

_Note that the bot currently does not support encrypted channels.__

## General usage

Commands are just text messages. They usually start with `!`. (That is, unless
the command prefix has been changed in the [configuration file](configuration-core.md)).
For example, the help command is a text message that looks like this: `!help`.
Once a user types such a command into the chat, the bot will react after a few
seconds by replying with another text message.

## Commands

The following list assumes that the command prefix is `!`. If you changed it to
something else, the commands have to be adjusted accordingly.

### Core commands

Core commands influence how the bot works or provide information about the bot
itself:

* `!help` - _(since version 0.0.6)_ shows a help message containing all
  available commands and a short explanation for each command. _Since version
  0.7.0_, it can also show a more detailed help for a specific command. For
  example, `!help wiki` or `!help !wiki` will show detailed help for the `!wiki`
  command.
* `!stop` - _(since version 0.0.5)_ stops the bot. Note that only users that are
  mentioned in the [configuration file](configuration-core.md) are allowed to
  stop the bot.
  _(Before version 0.0.11 any user could initiate the bot shutdown.)_
* `!rooms` - _(since version 0.1.7)_ shows a list of rooms where the bot is
  active. Only users that are allowed to stop the bot can get a list.
* `!leave` - _(since version 0.1.7)_ makes the bot leave a Matrix room, e. g.
  the command `!leave !id_of_room:example.com` would make the bot leave the
  room with the id `!id_of_room:example.com`. If no room id is given, i. e. the
  command is just `!leave`, then the bot is told to leave the room where the
  command was sent.
  Only users that are allowed to stop the bot or have the power levels to ban or
  kick users out of the corresponding room can make the bot leave a room.
* `!version` - _(sinve version 0.0.5)_ shows version information for the bot
* `!whoami` - _(since version 0.0.8)_ show the Matrix id of the user that sent
  the request

### Wikipedia commands

* `!wiki` - _(since version 0.0.7)_ same as `!wikien`
* `!wikics` - _(since version 0.1.0)_ gets extract from an article on the Czech
  Wikipedia, e. g. `!wikics Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikicy` - _(since version 0.1.0)_ gets extract from an article on the Welsh
  Wikipedia, e. g. `!wikicy Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikide` - _(since version 0.0.7)_ gets extract from an article on the German
  Wikipedia, e. g. `!wikide Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikiel` - _(since version 0.1.0)_ gets extract from an article on the Greek
  Wikipedia, e. g. `!wikiel Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikien` - _(since version 0.0.7)_ gets extract from an article on the
  English Wikipedia, e. g. `!wikien Albert Einstein` will show an extract from
  the article about the scientist Albert Einstein
* `!wikies` - _(since version 0.0.7)_ gets extract from an article on the
  Spanish Wikipedia, e. g. `!wikien Albert Einstein` will show an extract from
  the article about the scientist Albert Einstein
* `!wikifr` - _(since version 0.0.7)_ gets extract from an article on the French
  Wikipedia, e. g. `!wikifr Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikiit` - _(since version 0.0.7)_ gets extract from an article on the
  Italian Wikipedia, e. g. `!wikiit Albert Einstein` will show an extract from
  the article about the scientist Albert Einstein
* `!wikinl` - _(since version 0.1.0)_ gets extract from an article on the Dutch
  Wikipedia, e. g. `!wikinl Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikipl` - _(since version 0.1.0)_ gets extract from an article on the Polish
  Wikipedia, e. g. `!wikipl Albert Einstein` will show an extract from the
  article about the scientist Albert Einstein
* `!wikipt` - _(since version 0.1.0)_ gets extract from an article on the
  Portuguese Wikipedia, e. g. `!wikipt Albert Einstein` will show an extract
  from the article about the scientist Albert Einstein
* `!wikiru` - _(since version 0.0.7)_ gets extract from an article on the
  Russian Wikipedia, e. g. `!wikiru Albert Einstein` will show an extract from
  the article about the scientist Albert Einstein
* `!wikitr` - _(since version 0.1.0)_ gets extract from an article on the
  Turkish Wikipedia, e. g. `!wikitr Albert Einstein` will show an extract from
  the article about the scientist Albert Einstein
* `!wikiuk` - _(since version 0.1.0)_ gets extract from an article on the
  Ukrainian Wikipedia, e. g. `!wikiuk Альберт Ейнштейн` will show an extract
  from the article about the scientist Albert Einstein

### Entertainment

These commands are merely for entertainment purposes.

* `!fortune` - _(since version 0.0.10)_ displays a random quote
* `!fortunes` - _(since version 0.0.10)_ alias of the fortune command
* `!giphy` - _(since version 0.6.3)_ show a random GIF file matching a given
  keyword. For example, `!giphy cat` will show a random GIF of a cat.
* `!xkcd` - _(since 0.1.1)_ show a random comic from xkcd.com. If you want to
  get a particular comic, use its number after the command. For example,
  `!xkcd 1` will show the very first comic of xkcd.

A fortune command version that uses the popular `cowsay` to display messages may
(or may not) be added in the future.

### Translation

The following commands allow text translation between different languages:

* `!tr` - _(since version 0.4.0)_ translates text from one language to another,
  e. g. `!tr en de Hello world!` will translate the text "Hello world!" from
  English (`en`) to German (`de`).
* `!tr-lang` - _(since version 0.4.0)_ shows a list of available languages and
  their corresponding language codes for the `!tr` command.

Note that these commands only work, if a translation server (and possibly an API
key) has been set in botvinnik's configuration file.

### Debian package search

The following commands search for available [Debian](https://www.debian.org/)
packages:

* `!deb` - _(since version 0.1.3)_ alias for package searches for the current
  stable release of Debian, currently equal to the `!deb12` command
* `!deb13` - _(since version 0.6.2)_ searches for packages for Debian 13
  ("trixie"), e.g. `!deb13 grep` will find Debian 13 packages where "grep" is
  part of the name
* `!deb12` - _(since version 0.3.16)_ searches for packages for Debian 12
  ("bookworm"), e.g. `!deb12 grep` will find Debian 12 packages where "grep" is
  part of the name
* `!deb11` - _(since version 0.1.3)_ searches for packages for Debian 11
  ("bullseye"), e.g. `!deb11 grep` will find Debian 11 packages where "grep" is
  part of the name
* `!deb10` - _(since version 0.1.3)_ searches for packages for Debian 10
  ("buster"), e.g. `!deb10 grep` will find Debian 10 packages where "grep" is
  part of the name
* `!deb9` - _(since version 0.1.3)_ searches for packages for Debian 9
  ("stretch"), e.g. `!deb9 grep` will find Debian 9 packages where "grep" is
  part of the name
* `!deb8` - _(since version 0.1.3)_ searches for packages for Debian 8
  ("jessie"), e.g. `!deb8 grep` will find Debian 8 packages where "grep" is
  part of the name

### Information about Unix programs and programming languages

The following commands search [cheat.sh](https://cheat.sh/) for information,
so-called cheat sheets, about common Unix programs and / or programming
languages:

* `!cheat` - _(since version 0.3.9)_ displays a cheat sheet for the given Unix
   program / command, e. g. `!cheat grep` will show a list of common usage
   examples for the `grep` tool, while `!cheat java json parse` will show a
   small code example how to parse JSON when using the Java programming language
* `!cheats` - _(since version 0.3.9)_ alias for `!cheat`

### Numeric conversion commands

The following commands can convert between binary, decimal and hexadecimal
numbers. For example, `!bin2dec 1101` will convert the binary number 1101 to
its decimal representation.

* `!bin2dec` - _(since version 0.1.5)_ converts a binary number to a decimal
  number
* `!bin2hex` - _(since version 0.1.5)_ converts a binary number to a hexadecimal
  number
* `!dec2bin` - _(since version 0.1.5)_ converts a decimal number to a binary
  number
* `!dec2hex` - _(since version 0.1.5)_ converts a decimal number to a
  hexadecimal number
* `!hex2bin` - _(since version 0.1.5)_ converts a hexadecimal number to a binary
  number
* `!hex2dec` - _(since version 0.1.5)_ converts a hexadecimal number to a
  decimal number

### Connection test commands

Connection tests commands are there to test connection availability.

* `!ping` - _(since version 0.0.9)_ shows the time it took the message to reach
  the bot

**Note:** Please be aware that the bot user (like every other Matrix user) has
to obey the rate limits for sending messages. Therefore, if you spam ping
commands (or any other commands) too much, the bot will not be able to answer
all requests, because the Matrix server will just stop accepting messages for a
certain period of time. So please do not spam bot commands.

### Other commands

* `!corona` - _(since version 0.1.4)_ gets current COVID-19 case numbers. The
  country for which to get the data can be specified either by its English name
  (e. g. `!corona Germany`) or by its three letter code from ISO 3166 (e. g.
  `!corona DEU` for Germany's data). If you want to show summarized worldwide
  case numbers, just type `!corona world` or `!corona all` to show the total
  amount of COVID-19 cases in the world. Worldwide data is available _since
  version 0.1.10_.
