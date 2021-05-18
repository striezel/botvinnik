#!/usr/bin/env bash

#  stop.sh - script to stop the Docker container for botvinnik
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

# Check whether Docker exists / daemon is running and user has permission to
# use it.
docker info > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Error: It looks like Docker is not installed or you do not have the"
  echo "required permissions to use Docker."
  exit 1
fi

# Stop the container.
docker container stop matrix-bot-instance
if [ $? -ne 0 ]
then
  echo "ERROR: Could not stop Docker container!"
  exit 1
fi
