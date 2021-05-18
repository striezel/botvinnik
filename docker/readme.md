# Docker management scripts for botvinnik

This directory contains a few utility scripts to help with the management of the
Docker image and container for botvinnik.

## build.sh

Builds a new Docker image, tagging it as `botvinnik:vX.Y.Z`, where `vX.Y.Z` is
the latest tag reachable from the current commit. The build uses the option 
`--no-cache` of the Docker client to always build a fresh image.

## start.sh

Starts the Docker image of botvinnik, using the name `matrix-bot-instance` for
the container. This will fail, if a container with the same name is already
running, e. g. when a previous version of the bot already runs as container.

The container will automatically restart on failures (e. g. when the Matrix
homeserver is not reachable for a while). To stop the container, type

```shell
docker container stop matrix-bot-instance
```

or just use the `stop.sh` script.

## stop.sh

Stops the Docker container of botvinnik (i. e. it stops the container named
`matrix-bot-instance`).
