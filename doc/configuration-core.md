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

* **matrix.homeserver** - _(since 0.0.1)_ host name of the server where the
  Matrix instance runs
* **matrix.userid** - _(since 0.0.1)_ user id of the Matrix user (e. g.
  `@alice:example.tld`)
* **matrix.password** - _(since 0.0.1)_ password for the Matrix user
* **command.prefix** - _(since 0.0.5)_ prefix for text commands (e. g. if you
  have a hypothetical command `foo` and the prefix is set to `!`, then the text
  message `!foo` will trigger that command)

# Example of a complete configuration file

The following example is a complete core configuration file for the
botvinnik program (as of version 0.0.5):

    # This line is a comment and will be ignored by the program.
    #And so is this line.

    # Matrix server login settings
    matrix.homeserver=https://matrix.example.tld/
    matrix.userid=@alice:matrix.example.tld
    matrix.password=secret, secret, top(!) secret
    command.prefix=!
