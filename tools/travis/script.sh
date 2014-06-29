#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	make -j2 || exit 1


elif [ "$PLATFORM" == "android" ]; then
	cd tools/android

	# Clean (for local testing)
	android-ndk-r9d/ndk-build clean || exit 1
	ant clean || exit 1

	# Build
	android-ndk-r9d/ndk-build || exit 1
	ant debug || exit 1

	cd ../..


else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
