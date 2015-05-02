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


elif [ "$PLATFORM" == "android" ]; then
	cd tools/android;
	
	# Needed for debugging and image resizing
	sudo apt-get update -qq || exit 1
	sudo apt-get install -qq --force-yes ant imagemagick >/dev/null || exit 1;

	# Needed for x64 machines (packages removed after Ubuntu 12.04 LTS)
	if [ `uname -m` = x86_64 ] && [ `lsb_release -rs` == "12.04" ]; then
		sudo apt-get install -qq --force-yes libgd2-xpm ia32-libs ia32-libs-multiarch >/dev/null || exit 1;
	fi

	# A Java JDK is also needed, e.g.:
	# sudo apt-get install openjdk-7-jdk

	# Download and extract SDK
	SDK_FILE="android-sdk_r24.0.2-linux.tgz"
	SDK_LINK="http://dl.google.com/android/${SDK_FILE}"
	if [ ! -f ${SDK_FILE} ]; then
		wget ${SDK_LINK} || exit 1
	else
		# File exists locally already, check if remote file exists
		wget --spider ${SDK_LINK} || exit 1;
	fi
	# Clean (for local testing)
	rm -rf android-sdk-linux/
	tar -zxf ${SDK_FILE} || exit 1
	export ANDROID_HOME=`pwd`/android-sdk-linux
	export PATH=${PATH}:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools
	echo "sdk.dir=${ANDROID_HOME}" > local.properties

	# Install required Android components
	echo 'y' | android update sdk --all --filter platform-tools,build-tools-22.0.0,android-10 --no-ui --force >/dev/null || exit 1

	# Download and extract NDK
	NDK_FILE="android-ndk64-r10b-linux-x86_64.tar.bz2"
	NDK_LINK="http://dl.google.com/android/ndk/${NDK_FILE}"
	if [ ! -f ${NDK_FILE} ]; then
		wget ${NDK_LINK} || exit 1;
	else
		# File exists locally already, check if remote file exists
		wget --spider ${NDK_LINK} || exit 1;
	fi
	# Clean (for local testing)
	rm -rf android-ndk-r*/
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
