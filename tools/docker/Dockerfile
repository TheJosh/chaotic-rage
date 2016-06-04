# Base docker image
FROM ubuntu:16.04
MAINTAINER Magnus Espersson <magnusespersson@gmail.com>

# Install Chaotic Rage
RUN apt-get update
RUN apt-get install -y software-properties-common
RUN apt-add-repository -y ppa:chaotic-rage/chaoticrage
RUN apt-get update
RUN apt-get install -y chaoticrage

# Run Chaotic Rage
CMD bash -c "/usr/games/chaoticrage --audio null --render ascii --campaign ep1"
