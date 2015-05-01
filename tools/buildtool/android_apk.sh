#!/bin/bash
set -e

cd tools/android

~/android-ndk-r9d/ndk-build

ant debug

mv "bin/ChaoticRage-debug.apk" "$DESTDIR/chaoticrage-android-$VERSION.apk"
