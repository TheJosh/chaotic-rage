#!/bin/bash


if [ "$PLATFORM" == "linux" ]; then
	sudo apt-get install -qq lftp
	make dist VERSION=travis
	echo lftp -c "open 'ftp://$FTP_USER:$FTP_PASS@chaoticrage.com'; cd nightly; put chaotic-rage-travis.tar.bz2 -o 'chaotic-rage-linux-$TRAVIS_BRANCH.tar.bz2'"
	
	
elif [ "$PLATFORM" == "android" ]; then
	# android deploy coming soon :)
	# ant debug
	#echo lftp -c "open 'ftp://$FTP_USER:$FTP_PASS@chaoticrage.com'; cd nightly; lcd tools/android/bin/; put chaotic-rage-debug.apk -o 'chaotic-rage-android-$TRAVIS_BRANCH.apk'"
	
	
else
	echo 'Unknown $PLATFORM variable'
fi
