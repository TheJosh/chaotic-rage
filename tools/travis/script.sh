#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	make || exit 1
	
	
elif [ "$PLATFORM" == "android" ]; then
	cd tools/android
	android-ndk-r9b/ndk-build || exit 1
	cd ../..
	
	
else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
