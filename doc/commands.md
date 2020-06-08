# botvinnik: Available bot commands

botvinnik provides a (still rather small) number of commands for Matrix users
in the channels where the bot has joined. Note that joining channels has to be
done manually (as of version 0.0.6).

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
  available commands and a short explanation for each command
* `!stop` - _(since version 0.0.5)_ stops the bot. Note that only users that are
  mentioned in the [configuration file](configuration-core.md) are allowed to
  stop the bot.
  _(Before version 0.0.11 any user could initiate the bot shutdown.)_
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
* `!xkcd` - _(since 0.1.1)_ show a random comic from xkcd.com

A fortune command version that uses the popular `cowsay` to display messages may
(or may not) be added in the future.

### Debian package search

The following commands search for available [Debian](https://www.debian.org/)
packages:

* `!deb` - _(since version 0.1.3)_ alias for `!deb10`
* `!deb10` - _(since version 0.1.3)_ searches for packages for Debian 10
  ("buster"), e.g. `!deb10 grep` will find Debian 10 packages where "grep" is
  part of the name
* `!deb9` - _(since version 0.1.3)_ searches for packages for Debian 9
  ("stretch"), e.g. `!deb9 grep` will find Debian 9 packages where "grep" is
  part of the name
* `!deb8` - _(since version 0.1.3)_ searches for packages for Debian 8
  ("jessie"), e.g. `!deb8 grep` will find Debian 8 packages where "grep" is
  part of the name

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
  (e. g. `!corona Germany`) or by its two letter code from ISO 3166 (e. g.
  `!corona DE` for Germany's data)
