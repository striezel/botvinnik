# botvinnik - a simple bot for Matrix chats

botvinnik is a bot that hooks into a Matrix home server and provides some (more
or less) useful additions.

It is still in an early stage of development.

See the [documentation](doc/readme.md) for more information on how to get
started with the bot.

## Build status

* GitHub workflows:
  * [![GitHub CI Clang status](https://github.com/striezel/botvinnik/workflows/GitHub%20CI%20with%20Clang%208,%209,%2010/badge.svg)](https://github.com/striezel/botvinnik/actions)
  * [![GitHub CI GCC status](https://github.com/striezel/botvinnik/workflows/GitHub%20CI%20with%20GCC%208,%209,%2010/badge.svg)](https://github.com/striezel/botvinnik/actions)
* GitLab CI:
[![GitLab pipeline status](https://gitlab.com/striezel/botvinnik/badges/master/pipeline.svg)](https://gitlab.com/striezel/botvinnik/)
* Travis CI:
[![Build Status](https://travis-ci.org/striezel/botvinnik.svg?branch=master)](https://travis-ci.org/striezel/botvinnik)

## Building from source

### Prerequisites

To build botvinnik from source you need a C++ compiler with support for C++17,
CMake 2.8 or later, the cURL library (>=7.17) and the SQLite 3 library.
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

Copyright 2020  Dirk Stolle

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
