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
the reference implementation.

Probably the easiest way to sign up and try Matrix out is to use Riot, a
web-based client. Go to <https://riot.im/app> to get started. This will allow
you to sign up for a new account on Matrix.org.

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
