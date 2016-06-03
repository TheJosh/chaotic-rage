#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev libfreetype6-dev zlib-bin freeglut3-dev >/dev/null || exit 1

	cd tools/linux;
	./assimp.sh >/dev/null || exit 1;
	./bullet.sh >/dev/null || exit 1;
	./sdl2.sh >/dev/null || exit 1;
	./sdl2_image.sh >/dev/null || exit 1;
	./sdl2_mixer.sh >/dev/null || exit 1;
	./sdl2_net.sh >/dev/null || exit 1;
	cd ../..;


else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
