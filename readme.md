# botvinnik - a simple bot for Matrix chats

botvinnik is a bot that hooks into a [Matrix](https://matrix.org/) homeserver
and provides some (more or less) useful additions.

See the [documentation](doc/readme.md) for more information on how to get
started with the bot.

_Note: So far the bot it has only been tested against the
[Synapse homeserver](https://github.com/matrix-org/synapse/)
which is the de-facto reference implementation of a Matrix homeserver. It may or
may not work with other homeserver implementations such as
[dendrite](https://github.com/matrix-org/dendrite) or
[Conduit](https://conduit.rs/)._

## Build status

* GitHub workflows:
  [![GitHub CI Clang status](https://github.com/striezel/botvinnik/workflows/Clang/badge.svg)](https://github.com/striezel/botvinnik/actions)
  [![GitHub CI GCC status](https://github.com/striezel/botvinnik/workflows/GCC/badge.svg)](https://github.com/striezel/botvinnik/actions)
  [![GitHub CI MSYS2 status](https://github.com/striezel/botvinnik/workflows/MSYS2/badge.svg)](https://github.com/striezel/botvinnik/actions)
* GitLab CI:
[![GitLab pipeline status](https://gitlab.com/striezel/botvinnik/badges/master/pipeline.svg)](https://gitlab.com/striezel/botvinnik/)

## Building from source

### Prerequisites

To build botvinnik from source you need a C++ compiler with support for C++17,
CMake 3.8 or later, the cURL library (>=7.17) and the SQLite 3 library.
pkg-config is required to make it easier to find compiler options for the
installed libraries. Additionally, the program uses Catch (C++ Automated Test
Cases in Headers) to perform some tests.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed be typing

    apt-get install catch cmake g++ git libcurl4-gnutls-dev libsqlite3-dev pkg-config

or

    yum install catch cmake gcc-c++ git libcurl-devel libsqlite3-devel pkgconfig

into a root terminal.

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

    git clone https://gitlab.com/striezel/botvinnik.git ./bvn
    cd bvn

That's it, you should now have the current source code on your machine.

### Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

    mkdir build
    cd build
    cmake ../
    # If you want the bot to send its User-Agent header to the Matrix server,
    # then use `cmake -DUSER_AGENT=ON ../` instead.
    make -j4

If you want to run the test suite, too, then there is one more step:

    ctest -V

## Building from source on Windows

For information how to build botvinnik on Windows, see the
[MSYS2 build instructions](./doc/msys2-build.md).

## Building in Docker

This repository contains a `Dockerfile` that does a multistage build.
Be sure to replace the dummy values of the environment variables near the end of
the file to set the URL of your Matrix homeserver, the Matrix user ID and
password before building it.

### Prerequisites

Obviously, you will need a running Docker installation. Since the Dockerfile
uses a multistage build, at least version 17.05 of Docker is required.

### Build and run with Docker

To build the Docker image for botvinnik, type

```shell
docker build . -t botvinnik
```

in the root directory of this repository. After the image has been built, type

```shell
docker run -d --restart=unless-stopped --name=matrix-bot-instance botvinnik
```

to start the container. To stop it later, type

```shell
docker container stop matrix-bot-instance
```

You can also use the [Bash scripts in the `docker/` directory](./docker/readme.md),
if you want to avoid typing those commands out manually.

## Usage

    botvinnik [OPTIONS]

    options:
      -? | --help            - shows this help message
      -v | --version         - shows version information
      -c FILE | --conf FILE  - sets the file name of the configuration file to use
                               during the program run. If this option is omitted,
                               then the program will search for the configuration
                               in some predefined locations.

## History of changes

A changelog is provided as [separate file](./changelog.md).

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
