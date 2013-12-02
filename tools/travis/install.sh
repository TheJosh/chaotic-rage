#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev liblua5.1-0-dev libfreetype6-dev zlib-bin freeglut3-dev >/dev/null || exit 1
	
	cd tools/linux;
	./assimp.sh || exit 1;
	./bullet.sh || exit 1;
	./sdl2.sh || exit 1;
	./sdl2_image.sh || exit 1;
	./sdl2_mixer.sh || exit 1;
	./sdl2_net.sh || exit 1h;
	cd ../..;
	
	
elif [ "$PLATFORM" == "android" ]; then
	cd tools/android;
	
	# Need this for x64 machines
	if [ `uname -m` = x86_64 ]; then
		sudo apt-get update -qq || exit 1
		sudo apt-get install -qq --force-yes libgd2-xpm ia32-libs ia32-libs-multiarch >/dev/null || exit 1;
	fi
	
	# Download and extract SDK
	wget http://dl.google.com/android/android-sdk_r22.3-linux.tgz || exit 1
	tar -zxf android-sdk_r22.3-linux.tgz || exit 1
	export ANDROID_HOME=`pwd`/android-sdk-linux
	export PATH=${PATH}:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools
	echo "sdk.dir=${ANDROID_HOME}" > local.properties
	
	# Install required Android components
	echo 'y' | android update sdk -a --filter platform-tools --no-ui --force >/dev/null || exit 1
	echo 'y' | android update sdk -a --filter build-tools-19.0.0 --no-ui --force >/dev/null || exit 1
	echo 'y' | android update sdk -a --filter android-10 --no-ui --force >/dev/null || exit 1
	
	# Download and extract NDK
	wget http://dl.google.com/android/ndk/android-ndk-r9b-linux-x86_64.tar.bz2 || exit 1;
	tar -xjf android-ndk-r9b-linux-x86_64.tar.bz2 || exit 1;
	
	# Install and prepare all the libs we need for android builds
	./prepare.sh || exit 1
	
	cd ../..;
	
	
else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
