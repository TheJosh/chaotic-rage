#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get update -qq
	sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev liblua5.1-0-dev libfreetype6-dev zlib-bin freeglut3-dev
	
	cd tools/linux;
	./assimp.sh;
	./bullet.sh;
	./sdl2.sh;
	./sdl2_image.sh;
	./sdl2_mixer.sh;
	./sdl2_net.sh;
	cd ../..;
	
	
elif [ "$PLATFORM" == "android" ]; then
	cd tools/android;
	wget http://dl.google.com/android/ndk/android-ndk-r9b-linux-x86_64.tar.bz2;
	tar -xjf android-ndk-r9b-linux-x86_64.tar.bz2;
	cd ../..;
	
	
else
	echo 'Unknown $PLATFORM variable'
fi
