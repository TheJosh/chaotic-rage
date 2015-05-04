#!/bin/bash


ps aux
free -m -t

sudo /etc/init.d/mysql stop
sudo /etc/init.d/postgresql stop
/bin/sync

ps aux
free -m -t


if [ "$PLATFORM" == "linux" ]; then
	make -j2 || exit 1


elif [ "$PLATFORM" == "android" ]; then
	cd tools/android

	# Clean (for local testing)
	android-ndk-r*/ndk-build clean || exit 1
	ant clean || exit 1

	# Build
	android-ndk-r*/ndk-build -j2 || exit 1
	ant debug || exit 1

	cd ../..


else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
