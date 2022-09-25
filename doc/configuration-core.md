# botvinnik: core configuration file

The configuration file of botvinnik is the place where the core settings like
the Matrix home server or the user id and password for login are stored.
**Since this is sensitive information, take precaution that no other
users or only users who you trust can access the configuration file.** This can
usually be achieved by setting proper file permissions and file ownership for
the configuration file. (Something along the lines of `chmod 600 FILE_NAME` is
the usual approach, together with `chown user:group FILE_NAME`.)

# Location of the configuration file

The location of the configuration file can be specified via the command line
option `-c`. For example

    /path/to/botvinnik -c /home/user/some/path/bvn.conf

will tell the program to look for the configuration file
`/home/user/some/path/bvn.conf`. If no such option is given, the program will
look for the configuration file in these predefined locations, in the given
order:

* `user's home directory`/.bvn/bvn.conf
* `user's home directory`/.botvinnik/bvn.conf
* `user's home directory`/.botvinnik/botvinnik.conf
* /etc/botvinnik/botvinnik.conf
* /etc/botvinnik/bvn.conf
* /etc/bvn/bvn.conf
* ./botvinnik.conf
* ./bvn.conf

If none of these files exist and no location is given for the configuration
file, then the program will exit with a non-zero exit code.

# Configuration elements

## General format

Configuration settings are basically key-value pairs, where the key is specified
first, followed by an equals sign (`=`), followed by the value of the setting.
For example, the line

    matrix.homeserver=https://localhost/

sets the value of `matrix.homeserver` to `https://localhost/`.

## Comments

The configuration file allows comment lines. Comment lines are lines where the
first character is `#`. These lines are ignored by the application. A line has
to be either a comment or configuration data, you cannot mix both on the same
line.

Completely empty lines are ignored, too.

## Matrix server login settings

The following Matrix-related settings are recognized in the configuration file:

* **matrix.homeserver** - _(since 0.0.1, required)_ host name of the server
  where the Matrix instance runs (e. g. `https://matrix.example.tld/`)
* **matrix.userid** - _(since 0.0.1, required)_ user id of the Matrix user
  (e. g. `@alice:example.tld`)
* **matrix.password** - _(since 0.0.1, required)_ password for the Matrix user

All of these three settings are required to be present in any valid
configuration file, because these are essential for connecting to the Matrix
homeserver. The bot will post its replies as the Matrix user specified by the
**matrix.userid** setting. It is therefore advisable (although not required) to
create a separate Matrix user that is just used by the bot.

## Bot management settings

* **command.prefix** - _(since 0.0.5, optional)_ prefix for text commands (e. g.
  if you have a hypothetical command `foo` and the prefix is set to `!`, then
  the text message `!foo` will trigger that command); defaults to `!` if not set
* **command.deactivate** - _(since 0.5.0, optional)_ specifies a bot command
  that will be deactivated, i. e. it will not be available when the bot is
  running. For example, the line `command.deactivate=ping` would deactivate the
  `!ping` command. This option can be given multiple times to deactivate more
  than one command.

  _Note:_ Some commands like `!help`, `!stop` and others that are essential to
  operate the bot cannot be deactivated. The bot will not start if you attempt
  to deactivate such a command.
* **bot.stop.allowed.userid** - _(since 0.0.11, required)_ user id of a Matrix
  user that is allowed to stop the bot (e. g. `@alice:example.tld`). This
  setting may occur multiple times with different user ids to allow more users
  to stop the bot. At least one user id must be specified. Furthermore, the user
  id from the setting **matrix.userid** will be added to this list
  automatically, if it has not been specified as user allowed to stop the bot.
* **bot.sync.allowed_failures** - _(since 0.2.0, optional)_ the number of
  allowed Matrix synchronization failures within the last 32 synchronization
  attempts before the bot exits. Allowed range is from 0 (zero) to 31. This
  setting can be used to avoid that the bot stops when the network connection
  may occasionally be down for a few seconds. One allowed failure is approx. the
  equivalent of five seconds network downtime. (This time may change, if the
  setting **bot.sync.delay_milliseconds** is set to something else than 5000.)
  Default value of allowed failures is twelve, if it is not set explicitly.

  Set this to zero, if you want to restore the behaviour used in versions before
  0.2.0. However, a value of zero means that the bot stops whenever there is a
  small, short network downtime, so this is not advisable.
* **bot.sync.delay_milliseconds** - _(since 0.5.0, optional)_ the delay between
  two consecutive Matrix synchronization requests in milliseconds. (One second
  consists of 1000 milliseconds, e. g. 5000 milliseconds are 5 seconds.)
  Allowed range is from 100 (0.1 seconds) to 30000 (30 seconds). Values outside
  of this range will be replaced by the allowed minimum or maximum - whatever is
  closer. Default value is 5000, if it is not set explicitly.

  Primary purpose of this setting is to allow a balance between too high load on
  the Matrix server due to too many synchronization requests from the bot on the
  one side and a bot that is responding to commands too slowly on the other
  side.

## Translation server settings

The following settings control how the text translation commands work:

* **libretranslate.server** - _(since 0.4.0, optional)_ URL of the
  LibreTranslate server to use for translation commands. If this is omitted,
  then it is assumed to be empty. Empty server URL means no translation will be
  available.
* **libretranslate.apikey** - _(since 0.4.0, optional)_ API key for the
  LibreTranslate server. If this setting is omitted, then it is assumed to be
  empty. If this is empty, then no API key will be used in requests to the
  translation server. Some LibreTranslate instances will not work without an API
  key.

Note that at least a translation server URL has to be set, if you want the users
of the bot to be able to use the translation feature. Some servers also require
an API key, while other servers work without one.

A list of some possible LibreTranslate servers is available at
<https://github.com/LibreTranslate/LibreTranslate#mirrors>.

# Example of a complete configuration file

The following example is a complete core configuration file for the
botvinnik program (as of version 0.5.0):

    # This line is a comment and will be ignored by the program.
    #And so is this line.

    # Matrix server login settings
    matrix.homeserver=https://matrix.example.tld/
    matrix.userid=@alice:matrix.example.tld
    matrix.password=secret, secret, top(!) secret
    # bot management settings
    command.prefix=!
    bot.stop.allowed.userid=@bob:matrix.example.tld
    bot.sync.allowed_failures=12
    bot.sync.delay_milliseconds=5000
    # deactivate the !ping command
    command.deactivate=ping
    # translation server settings
    libretranslate.server=https://libretranslate.com
    libretranslate.apikey=abcdef1234567890
