#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq libgl1-mesa-dev libglu1-mesa-dev libglew-dev liblua5.1-0-dev libfreetype6-dev zlib-bin freeglut3-dev php5-cli >/dev/null || exit 1

	cd tools/linux;
	./assimp.sh >/dev/null || exit 1;
	./bullet.sh >/dev/null || exit 1;
	./sdl2.sh >/dev/null || exit 1;
	./sdl2_image.sh >/dev/null || exit 1;
	./sdl2_mixer.sh >/dev/null || exit 1;
	./sdl2_net.sh >/dev/null || exit 1;
	cd ../..;


elif [ "$PLATFORM" == "android" ]; then
	cd tools/android;
	
	# Needed for debugging, image resizing and php
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq --force-yes ant imagemagick php5-cli >/dev/null || exit 1;

	# Needed for x64 machines (packages removed between Ubuntu 12.04 LTS and Ubuntu 14.04 LTS)
	if [ `uname -m` = x86_64 ] && [ `lsb_release -rs` != "14.04" ]; then
		sudo apt-get install -qq --force-yes libgd2-xpm ia32-libs ia32-libs-multiarch >/dev/null || exit 1;
	fi

	# A Java JDK is also needed, e.g.:
	# sudo apt-get install openjdk-7-jdk

	# Download and extract SDK
	SDK_FILE="android-sdk_r23-linux.tgz"
	if [ ! -f ${SDK_FILE} ]; then
		wget http://dl.google.com/android/${SDK_FILE} || exit 1
	fi
	tar -zxf ${SDK_FILE} || exit 1
	export ANDROID_HOME=`pwd`/android-sdk-linux
	export PATH=${PATH}:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools
	echo "sdk.dir=${ANDROID_HOME}" > local.properties

	# Install required Android components
	echo 'y' | android update sdk -a --filter platform-tools --no-ui --force >/dev/null || exit 1
	echo 'y' | android update sdk -a --filter build-tools-20.0.0 --no-ui --force >/dev/null || exit 1
	echo 'y' | android update sdk -a --filter android-10 --no-ui --force >/dev/null || exit 1

	# Download and extract NDK
	NDK_FILE="android-ndk-r9d-linux-x86_64.tar.bz2"
	if [ ! -f ${NDK_FILE} ]; then
		wget http://dl.google.com/android/ndk/${NDK_FILE} || exit 1;
	fi
	tar -xjf ${NDK_FILE} || exit 1;

	# Install and prepare all the libs we need for android builds
	./prepare.sh || exit 1

	# Copy and shrink assets
	./assets.sh || exit 1

	cd ../..;


else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
