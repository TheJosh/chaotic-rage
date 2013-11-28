#!/bin/bash


# If the branch looks like a release, we feed into the release/ directory
# Otherwise it goes into nightly with the commit number too
if [ `echo "$TRAVIS_BRANCH" | grep "^1\.[0-9]"` ]; then
	DEST_NAME="$TRAVIS_BRANCH"
	DEST_DIR="release"
else
	DEST_NAME="$TRAVIS_BRANCH-$TRAVIS_COMMIT"
	DEST_DIR="nightly"
fi


# Do deploys
if [ "$PLATFORM" == "linux" ]; then
	# Linux
	sudo apt-get install -qq lftp
	make dist VERSION=travis
	lftp -c "open -u $FTP_USER,$FTP_PASS chaoticrage.com; cd '$DEST_DIR'; put chaotic-rage-travis.tar.bz2 -o 'chaotic-rage-linux-$DEST_NAME.tar.bz2'"
	
elif [ "$PLATFORM" == "android" ]; then
	# Android
	cd tools/android
	
	# Need this for x64 machines
	if [ `uname -m` = x86_64 ]; then
		sudo apt-get install -qq --force-yes libgd2-xpm ia32-libs ia32-libs-multiarch;
	fi
	
	# Download and extract SDK
	wget http://dl.google.com/android/android-sdk_r22.3-linux.tgz
	tar -zxf android-sdk_r22.3-linux.tgz
	export ANDROID_HOME=`pwd`/android-sdk-linux
	export PATH=${PATH}:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools
	
	# Install required Android components
	echo 'y' | android update sdk -a --filter platform-tools,android-10,build-tools-19.0.0 --no-ui --force
	
	# for ant
	echo "sdk.dir=${ANDROID_HOME}" > local.properties
	
	# do build
	ant debug
	
	# transfer file
	lftp -c "open -u $FTP_USER,$FTP_PASS chaoticrage.com; cd '$DEST_DIR'; put bin/SDLActivity-debug.apk -o 'chaotic-rage-android-$DEST_NAME.apk'"
	
	
else
	echo 'Unknown $PLATFORM variable'
fi


# Delete all but the 8 newest files
lftp -c "open -u $FTP_USER,$FTP_PASS chaoticrage.com; cd '$DEST_DIR'; cls -1 --sort=date" >list
head -n $(( $(wc -l list) - 8 )) list >rmlist
if [ -s rmlist ]; then
	$FILES=`cat rmlist | xargs`
	lftp -c "open -u $FTP_USER,$FTP_PASS chaoticrage.com; cd '$DEST_DIR'; rm $FILES" >list
fi
