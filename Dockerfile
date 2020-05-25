# Note: This file contains three environment variables near the end. Change
#       these to match your settings or you will not be able to connect the bot
#       to a Matrix homeserver.

FROM debian:10-slim
MAINTAINER Dirk Stolle <striezel-dev@web.de>

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y catch cmake g++ git libcurl4-gnutls-dev pkg-config
RUN mkdir -p /opt/bvn
COPY ./ /opt/bvn
WORKDIR /opt/bvn
RUN mkdir build && cd build && cmake .. && make -j4 && ctest
RUN ln -s /opt/bvn/build/src/botvinnik/botvinnik /usr/local/bin/botvinnik

# Create new user that will be used to run the program.
RUN useradd --create-home bot
USER bot
WORKDIR /home/bot
# Test that executable is reachable via PATH variable.
RUN botvinnik --version
RUN mkdir -p /home/bot/.bvn

# Note: Change these three vars to match your setup!
ENV HOMESERVER=https://matrix.org/
ENV BOT_USER=@alice-and-bob:charlie.tld
ENV BOT_PASSWORD=secret
# Generate configuration file.
RUN echo "matrix.homeserver=$HOMESERVER" > /home/bot/.bvn/bvn.conf && \
    echo "matrix.userid=$BOT_USER" >> /home/bot/.bvn/bvn.conf && \
    echo "matrix.password=$BOT_PASSWORD" >> /home/bot/.bvn/bvn.conf

CMD botvinnik
