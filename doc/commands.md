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

Available bot commands are:

* `!help` - _(since version 0.0.6)_ shows a help message containing all
  available commands and a short explanation for each command
* `!stop` - _(since version 0.0.5)_ stops the bot. Not that currently (as of
  version 0.0.6) any user can initiate the bot shutdown. That may change with a
  future version.
* `!version` - _(sinve version 0.0.5)_ show version information for the bot
