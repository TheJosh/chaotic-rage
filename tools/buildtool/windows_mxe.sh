#!/bin/sh

DISTTMP="chaoticrage-mxe-$VERSION"

# Build
make clean
make MXE=~/mxe

# Package
mkdir "$DISTTMP"
mv chaoticrage.exe "$DISTTMP"
cp -r data "$DISTTMP"
cp ~/mxe/usr/i686-w64-mingw32.static/lib/libassimp.dll "$DISTTMP"

# Compress
tar -cvjf "chaoticrage-mxe-$VERSION.tar.bz2" "$DISTTMP"
mv "chaoticrage-mxe-$VERSION.tar.bz2" "$DESTDIR"

# Cleanup
rm -r "$DISTTMP"
