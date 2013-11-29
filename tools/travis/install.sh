#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev liblua5.1-0-dev libfreetype6-dev zlib-bin freeglut3-dev || exit 1
	
	cd tools/linux;
	./assimp.sh || exit 1;
	./bullet.sh || exit 1;
	./sdl2.sh || exit 1;
	./sdl2_image.sh || exit 1;
	./sdl2_mixer.sh || exit 1;
	./sdl2_net.sh || exit 1h;
	cd ../..;
	
	
elif [ "$PLATFORM" == "android" ]; then
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq libpng12-dev || exit 1
	
	cd tools/android;
	wget http://dl.google.com/android/ndk/android-ndk-r9b-linux-x86_64.tar.bz2 || exit 1;
	tar -xjf android-ndk-r9b-linux-x86_64.tar.bz2 || exit 1;
	./prepare.sh || exit 1
	cd ../..;
	
	
else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
