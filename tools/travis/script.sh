#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	make
	
	
elif [ "$PLATFORM" == "android" ]; then
	cd tools/android
	./prepare.sh
	android-ndk-r9b/ndk-build
	cd ../..
	
	
else
	echo 'Unknown $PLATFORM variable'
fi
