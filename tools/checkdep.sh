#!/bin/bash


PACKAGES="";

#
# Tools
#

g++ --version >/dev/null || PACKAGES="$PACKAGES g++"

make --version >/dev/null || PACKAGES="$PACKAGES make"

flex --version >/dev/null || PACKAGES="$PACKAGES flex"


#
# Libs
#

sdl-config --version >/dev/null 2>/dev/null || PACKAGES="${PACKAGES} libsdl1.2-dev"

cat /usr/include/SDL/SDL_mixer.h >/dev/null 2>/dev/null || PACKAGES="$PACKAGES libsdl-mixer1.2-dev"

cat /usr/include/SDL/SDL_image.h >/dev/null 2>/dev/null || PACKAGES="$PACKAGES libsdl-image1.2-dev"

cat /usr/include/SDL/SDL_net.h >/dev/null 2>/dev/null || PACKAGES="$PACKAGES libsdl-net1.2-dev"

pkg-config zziplib --exists || PACKAGES="$PACKAGES libzzip-dev"

pkg-config libconfuse --exists || PACKAGES="$PACKAGES libconfuse-dev"

pkg-config gl --exists || PACKAGES="${PACKAGES} libgl1-mesa-dev"

pkg-config glu --exists || PACKAGES="$PACKAGES libglu1-mesa-dev"

pkg-config glew --exists || PACKAGES="$PACKAGES libglew-dev"

pkg-config lua5.1 --exists || PACKAGES="$PACKAGES liblua5.1-0-dev"

freetype-config --version >/dev/null || PACKAGES="$PACKAGES libfreetype6-dev"


if [ -n "$PACKAGES" ]; then
	echo "On Ubuntu? Run this:"
	echo "   sudo apt-get install ${PACKAGES}"
fi

