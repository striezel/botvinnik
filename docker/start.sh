#!/usr/bin/env bash

#  start.sh - script to start the latest Docker image for botvinnik
#
#  Copyright (C) 2021  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>

SCRIPT_DIR=$(dirname "$BASH_SOURCE")
ORIGIN_DIR=$(pwd)

cd "$SCRIPT_DIR/.." || exit 1

# Gets the latest tag.
TAG="$(git describe | cut -d '-' -f 1)"
if [ -z "$TAG" ]
then
  echo "ERROR: Could not get latest Git tag. Is this a Git repository?"
  cd "$ORIGIN_DIR"
  exit 1
fi

# Check whether Docker exists / daemon is running and user has permission to
# use it.
docker info > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Error: It looks like Docker is not installed or you do not have the"
  echo "required permissions to use Docker."
  exit 2
fi

# Does the image exist? (Redirect STDIN+STDERR to /dev/null.)
docker image inspect "botvinnik:$TAG" > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Info: Image botvinnik:$TAG does not exist, building it now."
  "$SCRIPT_DIR"/build.sh
  if [[ $? -ne 0 ]]
  then
    echo "Docker image build process failed!"
    cd "$ORIGIN_DIR"
    exit 3
  fi
fi

# Start container.
docker run -d --restart=unless-stopped --name=matrix-bot-instance "botvinnik:$TAG"
if [ $? -ne 0 ]
then
  echo "ERROR: Could not start Docker container!"
  cd "$ORIGIN_DIR"
  exit 4
fi
