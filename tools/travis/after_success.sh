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
	echo 'android deploy coming soon :)'
	#ant debug
	#echo lftp -c "open 'ftp://$FTP_USER:$FTP_PASS@chaoticrage.com'; cd $DEST_DIR'; lcd tools/android/bin/; put chaotic-rage-debug.apk -o 'chaotic-rage-android-$DEST_NAME.apk'"
	
	
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
