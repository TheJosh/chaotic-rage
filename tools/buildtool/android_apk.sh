#!/bin/bash
set -e

cd tools/android

# Build C code
~/android-ndk-r9d/ndk-build

# Buid Jar
ant release

# Sign with the release key
jarsigner \
	-verbose \
	-sigalg SHA1withRSA \
	-digestalg SHA1 \
	-keystore ~/android-keys/chaoticrage-release-key.keystore \
	-storepass `cat ~/android-keys/password` \
	bin/ChaoticRage-release-unsigned.apk \
	chaoticrage

# Align (reduces ram usage)
~/android-sdk-linux/tools/zipalign \
	-v 4 \
	bin/ChaoticRage-release-unsigned.apk \
	bin/ChaoticRage-release.apk

# Move it where it belongs
mv "bin/ChaoticRage-release.apk" "$DESTDIR/chaoticrage-android-$VERSION.apk"
