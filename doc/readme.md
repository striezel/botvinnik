# Documentation for botvinnik Matrix bot

botvinnik is (or rather: will be) a chat bot that uses the Matrix protocol for
communication and provides some helpful commands to the Matrix users.

A list of all available commands can be seen [here](commands.md).

# Preparations

In order to use the full potential of botvinnik you need the compiled program
(see the readme in the root directory for build instructions) and a Matrix
server instance where you have a valid login for the bot. You can either try a
[public Matrix server](https://www.hello-matrix.net/public_servers.php) or set
up your own server, e. g. by using [Synapse](https://github.com/matrix-org/synapse),
the reference implementation. Other Matrix server implementations like
[Conduit](https://conduit.rs/) or [dendrite](https://github.com/matrix-org/dendrite)
may also work, but they have not been tested.

Probably the easiest way to sign up and try Matrix out is to use Element, a
web-based client. Go to <https://app.element.io/> to get started. This will
allow you to sign up for a new account on Matrix.org.

## Note about Synapse configuration

If you are using the bot on a Matrix homeserver running Synapse 1.62.0 or
later, then please make sure that the `sync_response_cache_duration` setting in
the server's configuration is set to zero, or - if that is too low - to a value
not higher than a few seconds. Synapse's default value for that setting is two
minutes, which would mean that the relevant server responses for the bot are
cached for two minutes and the bot would have to wait up to two minutes before
getting an up-to-date response and making a command actually "visible" to the
bot.

Therefore, if the bot seems to be painfully slow, ask the server administrator
to check and possibly adjust the setting. E. g., the configuration should
contain something like this:

```
caches:
  sync_response_cache_duration: 0
```

For more information on cache settings, consult the
[documentation of Synapse](https://matrix-org.github.io/synapse/latest/usage/configuration/config_documentation.html#caches-and-associated-values).

# Configuration file

The configuration file is the place where some of the core settings like the
Matrix login information are stored. Since this is some sensitive information,
take precaution that no other users or only users who you trust can access the
configuration file. This can usually be achieved by setting proper file
permissions and file ownership for the configuration file.

Full details on the core configuration (supported settings, etc.) can be found
[here](configuration-core.md).

# Bot commands

An overview for all available bot commands can be found in a
[separate documentation file](commands.md).

# Ideas for improvements

A small (but not necessarily complete) list for future improvements can be seen
[here](possible-improvements.md).
