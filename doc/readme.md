# botvinnik - a simple bot for Matrix chats

botvinnik is a bot that hooks into a [Matrix](https://matrix.org/) homeserver
and provides some (more or less) useful additions via commands that let the bot
show the current weather and weather forecast for given locations, get extracts
from Wikipedia articles or show a random xkcd comic, just to name a few
examples.

See [getting-started.md](getting-started.md) for more information on how to
get started with the bot.

_Note: So far the bot it has only been tested against the
[Synapse homeserver](https://github.com/element-hq/synapse)
which is the de-facto reference implementation of a Matrix homeserver. It may or
may not work with other homeserver implementations such as
[dendrite](https://github.com/matrix-org/dendrite) or
[Conduit](https://conduit.rs/)._

## Usage

```
botvinnik [OPTIONS]

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c FILE | --conf FILE  - Sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
```

## History of changes

A changelog is provided in the file changelog.md.

## Copyright and Licensing

Copyright 2020, 2021, 2022, 2023, 2024  Dirk Stolle

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
